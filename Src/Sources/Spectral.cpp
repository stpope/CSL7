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
			mFFTSize(1024),
			mIFFTSize(1024),
			mIFFTHop(128),
			mIFFT(0),
			mIFFTBuf(),
			mCache(),
			mWinCnt() {
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

	SamplePtr inSamples = sndFile.mWavetable.buffer(0);	// load sound file
	int numSamps = sndFile.duration();
	int numWins = numSamps / hopSize;					// compute # of FFT windows
	Buffer input(1, blockSize);							// buffer to hold windowed input
	input.allocateBuffers();
	Buffer spectrum(1, blockSize * 2);					// buffer to hold spectral slice
	SamplePtr slice;
	HammingWindow window(blockSize);  					// Window to scale input
	FFT_Wrapper fft(blockSize, CSL_FFT_COMPLEX, CSL_FFT_FORWARD);
	
	printf("Vocoder analysis loop - %d windows, FFT len %d, hop %d\n", numWins, blockSize, hopSize);
	for (int i = 0; i < numWins; i++) {					// window loop
		SamplePtr inptr = input.buffer(0);				// copy input sample to windowing buffer
		SampleBuffer winPtr = window.window();			// ptr to window buffer
		memcpy(inSamples + (i * hopSize), inptr, blockSize * sizeof(Sample));
		for (int i = 0; i < blockSize; i++)				// apply signal window to buffer
			*inptr++ *= *winPtr++;
		SAFE_MALLOC(slice, Sample, blockSize * 2);		// set up output spectrum ptrs
		spectrum.setBuffer(0, slice);

		fft.nextBuffer(input, spectrum);					// execute the FFT

		mSpectra.push_back((SampleComplexVector) slice);	// store result in mSpectra
	}
	mFFTSize = mIFFTSize = blockSize;						// set-up vars for IFFT
	mIFFTHop = hopSize;
	mTimeScale = 1.0f;
	mPitchScale = 1.0f;
	mWinCnt = 0;
}

void Vocoder::setTimeScale(float val) {
	mTimeScale = val;
	mIFFTSize = mFFTSize * val;
	if (mIFFT != NULL)
		delete mIFFT;
	mIFFT = new FFT_Wrapper(mIFFTSize, CSL_FFT_COMPLEX, CSL_FFT_INVERSE);
}

void Vocoder::setPitchScale(float val) {
	mPitchScale = val;
}

void Vocoder::nextBuffer(Buffer & outputBuffer) noexcept(false) {
	if (outputBuffer.mNumFrames != mIFFTSize) {
		logMsg(kLogError,
			"Vocoder::nextBuffer # frames %d wrong for IFFT size %d (use a block resizer).",
			outputBuffer.mNumFrames, mIFFT->mSize);
		return;
	}
	mIFFTBuf.setSize(1, outputBuffer.mNumFrames);
	mIFFTBuf.setBuffer(0, (SampleBuffer) mSpectra[mWinCnt]);
	this->warpSpectrum();									// do optional spectrum warping

	mIFFT->nextBuffer(mIFFTBuf, outputBuffer);			// execute the IFFT via the wrapper

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




