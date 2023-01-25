//
// Spectral.cpp -- UnitGenerator for going to/from the spectral domain using FFTW
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "CSL_Includes.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

using namespace csl;

/// Forward FFT = analysis

// Constructor - Default size to the buffer size and flags to measure
// mBlockResizer(size) needed if different size than the default

FFT::FFT(UnitGenerator & in, int size, CSL_FFTType type)
			: Effect(in), mFFTSize(size), 
			  mWrapper(size, type, CSL_FFT_FORWARD), mInBuf(1, size), mWindowBuffer(0) {  
//	mSampleBuffer = (SampleBuffer) fftwf_malloc(sizeof(SampleBuffer) * mFFTSize);
	mWindow = new HammingWindow(mFFTSize);  ///< Window to scale input
	mWindowBuffer = mWindow->window();	///< ptr to window buffer
	mOverwriteOutput = false;			// leave the spectrum in the buffer by default
	mInBuf.allocateBuffers();
}

FFT::~FFT() {
    free(mWindow);
    free(mWindowBuffer);
}

// nextBuffer does the FFT -- note that we override the higher-level version of this method

void FFT::nextBuffer(Buffer& outputBuffer) noexcept(false) {
	unsigned numFrames = outputBuffer.mNumFrames;		// get buffer length

	pullInput(numFrames);							// get the input samples via Effect
													// Copy the input data into the buffer 
	memcpy(mInBuf.buffer(0), mInputPtr, numFrames * sizeof(Sample));

	SampleBuffer bufPtr = mInBuf.buffer(0);			// apply signal window to buffer
	SampleBuffer winPtr = mWindowBuffer;	
	for (int i = 0; i < mFFTSize; i++)
		*bufPtr++ *= *winPtr++;

	mWrapper.nextBuffer(mInBuf, outputBuffer);		// execute the FFT

	outputBuffer.mType = kSpectra;					// set the type flag of the output buffer
	this->changed((void *) & outputBuffer);			// signal dependents (if any) of my change
	return;
}

// --------------------------------------------------------------------------------------------
//
//// InvFFT = synthesis
//

IFFT::IFFT(int size, CSL_FFTType type) : UnitGenerator(),
              mFFTSize(size),
			  mWrapper(size, type, CSL_FFT_INVERSE),
              mInBuf() {
	SAFE_MALLOC(mSpectrum, SampleComplex, mFFTSize * 2);
//    logMsg("Alloc IFFT");
}

IFFT::~IFFT() { 
	SAFE_FREE(mSpectrum);
//    logMsg("Free IFFT");
}

void IFFT::binValue(int binNumber, float * outRealPart, float * outImagPart) {
	*outRealPart = cx_r(mSpectrum[binNumber]);
	*outImagPart = cx_r(mSpectrum[binNumber]);	
}

void IFFT::binValueMagPhase(int binNumber, float * outMag, float * outPhase) {
	float myReal, myComplex;
	binValue(binNumber, &myReal, &myComplex);
	*outMag = hypot(myReal, myComplex);
	*outPhase = (0.0 == myReal) ? 0 : atan(myComplex/myReal);
}

void IFFT::setBin(int binNumber, float realPart, float imagPart) {
#ifdef CSL_DEBUG
	logMsg("\t\tset [%d] = %5.3f @ %5.3f", binNumber, realPart, imagPart);
#endif
	cx_r(mSpectrum[binNumber]) = realPart;
	cx_i(mSpectrum[binNumber]) = imagPart;
}

void IFFT::setBins(float * real, float * imag) {
	for (int i = 0; i < mFFTSize; i++) {
		cx_r(mSpectrum[i]) = real[i];
		cx_i(mSpectrum[i]) = imag[i];		
	}
}

void IFFT::setBins(SampleComplexVector cmplxSpectrum) {
	for (int i = 0; i < mFFTSize; i++) {
		cx_r(mSpectrum[i]) = cx_r(cmplxSpectrum[i]);
		cx_i(mSpectrum[i]) = cx_i(cmplxSpectrum[i]);		
	}
}

void IFFT::setBins(SampleBuffer cmplxSpectrum) {
	for (int i = 0; i < mFFTSize; i += 2) {
		cx_r(mSpectrum[i]) = *cmplxSpectrum++;
		cx_i(mSpectrum[i]) = *cmplxSpectrum++;		
	}
}

void IFFT::setBins(int lower, int upper, float * real, float * imag) {
	if (lower < 0 || lower >= mFFTSize ) { return; } // It should throw an "out of range" exception, right?
	if (upper < 0 || upper >= mFFTSize ) { return; }
	if (upper < lower ) { return; }

	for (int i = lower; i < upper; i++) {
		cx_r(mSpectrum[i]) = real[i];
		cx_i(mSpectrum[i]) = imag[i];		
	}
}

void IFFT::setBinMagPhase(int binNumber, float mag, float phase) {
	float myReal, myComplex;
	myReal = mag * cosf(phase);
	myComplex = mag * sinf(phase);
	setBin(binNumber, myReal, myComplex);
}

void IFFT::setBinsMagPhase(float* mags, float* phases) {
	for (int i = 0; i < mFFTSize; i++) {
		cx_r(mSpectrum[i]) = mags[i] * cos(phases[i]);
		cx_i(mSpectrum[i]) = mags[i] * sin(phases[i]);
	}
}

// Do the IFFT::nextBuffer by calling the wrapper

void IFFT::nextBuffer(Buffer & outputBuffer) noexcept(false) {
	if (outputBuffer.mNumFrames != mWrapper.mSize) {
		logMsg(kLogError, 
			"IFFT::nextBuffer # frames %d wrong for IFFT size %d (use a block resizer).",
			outputBuffer.mNumFrames, mWrapper.mSize);
		return;
	}
	mInBuf.setSize(1, outputBuffer.mNumFrames);
	mInBuf.setBuffer(0, (SampleBuffer) mSpectrum);

	mWrapper.nextBuffer(mInBuf, outputBuffer);			// execute the IFFT via the wrapper
    
    if (outputBuffer.mNumChannels > 1) {
        for (unsigned i = 1; i < outputBuffer.mNumChannels; i++)
        memcpy(outputBuffer.buffer(i), outputBuffer.buffer(0), outputBuffer.mMonoBufferByteSize);
    }
}

#pragma mark Vocoder // ----------------------------------------------------------------------------------

///
/// Vocoder uses an FFT and an IFFT and allows several kinds of pitch-time warping.
/// This is a simple version that supports fixed pitch/time warp factors.
///

Vocoder::Vocoder(CSL_FFTType type)
		: UnitGenerator(),
			mType(type),
			mTimeScale(1.0),
			mPitchScale(1.0),
			mFFTSize(0),
			mInHop(0),
			mIFFTSize(0),
			mIFFTHop(0),
			mIFFT(0),
			mWinCnt(0),
			mNBufs(0) {
	mIFFTBuf.mAreBuffersAllocated = true;
	mIFFTBuf.mIsPopulated = true;
}

Vocoder::~Vocoder() {
	mSpectra.clear();
}

// Load and analyze a file into the spectrum buffer

void Vocoder::analyzeFile(string folder, string path, int blockSize, int hopSize) {
	SoundFile sndFile(folder + path);
	sndFile.dump();
	mFFTSize = blockSize;
	mInHop = hopSize;

	SamplePtr inSamples = sndFile.mWavetable.buffer(0);	// load sound file
	int numSamps = sndFile.duration();
	mNInWins = numSamps / hopSize;						// compute # of FFT windows
	Buffer input(1, mFFTSize);							// buffer to hold windowed input
	input.allocateBuffers();
	Buffer spectrum(1, mFFTSize * 2);						// buffer to hold spectral slice
	SamplePtr slice;										// temp float * for buffering
	TriangularWindow window(mFFTSize);  					// triangle window to scale input
	FFT_Wrapper fft(mFFTSize, CSL_FFT_COMPLEX, CSL_FFT_FORWARD);	// the FFT himself
	
	logMsg("Vocoder analysis loop - %d windows, FFT len %d, hop %d", mNInWins, mFFTSize, mInHop);
	for (int i = 0; i < mNInWins; i++) {					// window loop
		SamplePtr inptr = input.buffer(0);				// copy input sample to windowing buffer
		SampleBuffer winPtr = window.window();			// ptr to window buffer
		memcpy(inSamples + (i * mInHop), inptr, mFFTSize * sizeof(Sample));	// copy in samples to temp buf
		for (int i = 0; i < mFFTSize; i++)				// apply signal window to buffer
			*inptr++ *= *winPtr++;
		SAFE_MALLOC(slice, Sample, mFFTSize * 2);			// set up output spectrum ptrs
		spectrum.setBuffer(0, slice);
//		logMsg("FFT from %d, to %x\n", (i * mInHop), slice);

		fft.nextBuffer(input, spectrum);					// execute the FFT
		mSpectra.push_back((SampleComplexVector) slice);	// store result in mSpectra
	}
	mTimeScale = 1.0f;
	mPitchScale = 1.0f;
	mWinCnt = 0;
}

// set the time scale

void Vocoder::setTimeScale(float val) {
	mTimeScale = val;
}

// set the pitch scale and warp the stored spectra

void Vocoder::setPitchScale(float val) {
	mPitchScale = val;
	if (mSpectra.size() == 0) {
		printf("Don't call setPitchScale() before analysing a sound\n");
		return;
	}
	if (val != 1.0f)
		this->warpSpectrum();						// do spectrum warping
}

void Vocoder::setupIFFT(int ifftSize) {
	mIFFTSize = ifftSize;							// same sizes for now
	if (mIFFT != NULL)
		delete mIFFT;							// re-create FFT wrapper
	mIFFT = new FFT_Wrapper(mIFFTSize, CSL_FFT_COMPLEX, CSL_FFT_INVERSE);
	mTriWin = new TriangularWindow(mIFFTSize);
	mIFFTHop = mInHop * mTimeScale;
	mNBufs = (int) mTimeScale + 2;
	for (int i = 0; i < mNBufs; i++) {			// initialize output buffers for OLA
		Buffer* buf = new Buffer(1, mIFFTSize);
		buf->allocateBuffers();
		mIFFTBuffers.push_back(buf);
	}											// do first window of IFFT
	mIFFTBuf.setSize(1, mIFFTSize);
	mIFFTBuf.setBuffer(0, (SampleBuffer) mSpectra[0]);
	mIFFT->nextBuffer(mIFFTBuf, * mIFFTBuffers[0]);// execute the IFFT via the wrapper
	
	SamplePtr winP = mTriWin->window();			// apply tri window to sample buffer
	SamplePtr bufP = mIFFTBuffers[0]->buffer(0);
	for (int i = 0; i < mIFFTSize; i++)
		*bufP++ *= *winP++;

	mWinCnt = 1;
}

// Vocoder::nextBuffer - do more IFFT if necessary and overlap-add buffers into the output

void Vocoder::nextBuffer(Buffer & outputBuffer) noexcept(false) {
	if (outputBuffer.mNumFrames != mIFFTSize) {
		logMsg(kLogError,
			"Vocoder::nextBuffer # frames %d wrong for IFFT size %d (use a block resizer).",
			outputBuffer.mNumFrames, mIFFT->mSize);
		return;
	}
	int n = mWinCnt % mNBufs;
	if (mWinCnt == 0)
		return;
	mIFFTBuf.setBuffer(0, (SampleBuffer) mSpectra[mWinCnt]);	// ptr to spectral data
	mIFFT->nextBuffer(mIFFTBuf, * mIFFTBuffers[n]);			// execute the IFFT via the wrapper
	SamplePtr winP = mTriWin->window();						// apply tri window to sample buffer
	SamplePtr bufP = mIFFTBuffers[n]->buffer(0);
		for (int j = 0; j < mIFFTSize; j++)
			*bufP++ *= *winP++;
	if (mWinCnt == 0)
		return;
	SamplePtr outP;
	int o = (n == 0) ? mNBufs - 1 : n - 1;					// index ptrs o, p, q are n-1, n, n+1 with wrap-around
	int p = n;
	int q = (n == mNBufs - 1) ? 0 : n + 1;
														// loop over window buffers summing into out
	outP = outputBuffer.buffer(0);						// 2nd half of previous window
	bufP = mIFFTBuffers[o]->buffer(0) + (mIFFTSize / 2);
		for (int j = 0; j < (mIFFTSize / 2); j++)
			*bufP++ += *winP++;
	
	outP = outputBuffer.buffer(0);						// current window
	bufP = mIFFTBuffers[p]->buffer(0) ;
		for (int j = 0; j < mIFFTSize; j++)
			*bufP++ += *winP++;
	
	if (mWinCnt > 2)
		return;
	outP = outputBuffer.buffer(0) + (mIFFTSize / 2);		// 1st half of next window
	bufP = mIFFTBuffers[q]->buffer(0) ;
		for (int j = 0; j < (mIFFTSize / 2); j++)
			*bufP++ += *winP++;

	mWinCnt++;
}

// General fcn to process the spectrum before re-synthesis (override in subclasses for fancier processing)

void Vocoder::warpSpectrum() {

	if (mPitchScale == 1.0f)
		return;
//	SampleComplexPtr sPtr = (SampleComplexPtr) mIFFTBuf.buffer(0);
//	for (int i = 0; i < mFFTSize; i++) {
//		
//	}
}




