///
/// Spectral.h -- UnitGenerator for going to/from the spectral domain
/// These classes use the CSL FFT wrapper for FFTs.
///
/// See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#ifndef CSL_Spectral_H
#define CSL_Spectral_H

#include "CSL_Core.h"
#include "Window.h"

#include "FFT_Wrapper.h"

namespace csl {

///
/// Forward FFT unit generator is an Effect because it handles an input
/// It puts spectral frames in the output buffer and then broadcasts a change message, so clients are expected to observe it.
///

class FFT : public Effect {

public:
										/// Default size to the buffer size and flags to measure
	FFT(UnitGenerator & in, int size = CGestalt::blockSize(), CSL_FFTType type = CSL_FFT_COMPLEX);
	~FFT();
										/// we override the general-case version because this needs a mono input
	void nextBuffer(Buffer & outputBuffer) noexcept(false);	

	int fftSize() { return mFFTSize; }	/// no setter -- create a new FFT to change size

	bool mOverwriteOutput;				///< whether to replace the output with the input (or the spectrum) after signalling observers

protected:	
	int mFFTSize;						///< This should be unsigned, but is signed for compatability with FFTW
	FFT_Wrapper mWrapper;				///< actual FFT wrapper object
	Buffer mInBuf;						///< input buffer
	SampleBuffer mWindowBuffer;			///< Buffer to store window
    csl::HammingWindow * mWindow; 		///< the window itself
};

///
/// Inverse FFT is a generator that uses the IFFT for synthesis
///

class IFFT : public UnitGenerator {

public:
										/// Default size to the buffer size and flags to measure
	IFFT(int size = CGestalt::blockSize(), CSL_FFTType type = CSL_FFT_COMPLEX);
	~IFFT();
										/// no setter -- create a new IFFT to change size
	int fftSize() { return mFFTSize; }
										/// getter methods
	void binValue(int binNumber, float * outRealPart, float * outComplexPart);
	void binValueMagPhase(int binNumber, float * outMag, float * outPhase);
		
										/// set the values in the specified bin
	void setBin(int binNumber, float realPart, float imagPart);
	void setBins(float * real, float * imag);
	void setBins(SampleComplexVector cmplxSpectrum);
	void setBins(SampleBuffer cmplxSpectrum);
	void setBins(int lower, int upper, float * real, float * imag);
	void setBinMagPhase(int binNumber, float mag, float phase);
	void setBinsMagPhase(float * mags, float * phases);
	
	void nextBuffer(Buffer & outputBuffer) noexcept(false);

protected:	
	int mFFTSize;						///< This should be unsigned, but is signed for compatability with FFTW
	FFT_Wrapper mWrapper;				///< actual FFT wrapper object
	Buffer mInBuf;						///< input buffer
	SampleComplexPtr mSpectrum;			///< spectral data I accumulate (vector of complex)
};

///
/// Vocoder uses an FFT and an IFFT and allows several kinds of pitch-time warping.
/// This is a simple version that supports fixed pitch/time warp factors (which should be UGens).
/// This version reads a snd file for input, i.e., is non-interactive, but a subclass could be taught to process live input
///

class Vocoder : public UnitGenerator {

public:
										/// Default size to the buffer size and flags to measure
	Vocoder(CSL_FFTType type = CSL_FFT_COMPLEX);
	~Vocoder();
										/// Load and analyze a file into the spectrum buffer
	void analyzeFile(string folder, string path, int blockSize, int hopSize);
										/// Play back the transformed spectrum
	void nextBuffer(Buffer & outputBuffer) noexcept(false);

	void setTimeScale(float val);
	void setPitchScale(float val);
//	void setTimeScale(UnitGenerator & val);// ToDo
//	void setPitchScale(UnitGenerator & val);
	void setupIFFT(int ifftSize);			///< prep for the output after setting pitch/time warping

protected:
	virtual void warpSpectrum();			///< General fcn to process the spectrum before re-synthesis

	SampleComplexSpectra mSpectra;		///< Spectral data I accumulate (vector of complex arrays)
	float mTimeScale, mPitchScale;		///< ToDo: these should be UGens
	FFT_Wrapper * mIFFT;					///< IFFT wrapper object
	Buffer mIFFTBuf;						///< IFFT buffer
	unsigned mWinCnt;					///< Re-synthesis frame counter
	CSL_FFTType mType;					///< do I use real or complex FFT?
	std::vector <Buffer*>  mIFFTBuffers;	///< the array of windowed output buffers
	TriangularWindow * mTriWin;  			///< triangle window to scale output

	int mFFTSize, mInHop, mNInWins;		///< input block and hop sizes and # of FFT wunidows
	int mIFFTSize, mIFFTHop;				///< output block and hop sizes
	int mNBufs;							///< num out bufs (for time-stretching)
};

} // namespace csl


#endif
