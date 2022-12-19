///
/// AdditiveInstrument.h -- Sum-of-sines synthesis instrument class.
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// 
///
/// Accessors
///		"du", set_duration_f
///		"am", set_amplitude_f
///		"fr", set_freq_f
///		"po", set_position_f
///		"aa", set_attack_f	-- amplitude envelope ADSR
///		"ad", set_decay_f
///		"as", set_sustain_f
///		"re", set_release_f
///		"pt", set_partial_f
///		"ps", set_partials_f
///
/// OSC note formats (4 or 8 arguments):
/// 	dur, ampl, c_fr, pos
/// 	dur, ampl, c_fr, pos, att, dec, sus, rel

#ifndef INCLUDE_ADDITIVE_Instrument_H
#define INCLUDE_ADDITIVE_Instrument_H

#include "Instrument.h"
#include "Mixer.h"
#include "SHARC.h"

namespace csl  {

///
/// AdditiveInstrument
///

class AdditiveInstrument : public Instrument {
public:
	AdditiveInstrument();			///< Constructor
	AdditiveInstrument(SumOfSines & sos);
	AdditiveInstrument(unsigned numHarms, float noise);
	AdditiveInstrument(SHARCSpectrum & spect);

	AdditiveInstrument(AdditiveInstrument&);		///< copy constructor
	~AdditiveInstrument();
								/// Plug functions
	void setParameter(unsigned selector, int argc, void **argv, const char *types);
								/// Play functions
	void playOSC(int argc, void **argv, const char *types);
	
	void playNote(float dur = 1, float ampl = 1,
				float c_fr = 110, float pos = 0,
				float att = 0.05, float dec = 0.05, float sus = 0.5, float rel = 0.5);
	void playMIDI(float dur, int chan, int key, int vel);

	ADSR mAEnv;					///< amplitude envelope
	SumOfSines mSOS;				///< sum-of-sine oscillator
	Panner mPanner;				///< stereo panner
protected:
	void init();
};

///
/// SHARCAddInstrument - uses the SHARC spectra to create sum-of-sines players.
/// An example of adding features like attack chiff and vibrato with conditional compilation (messy, I know)
///

//#ifdef CSL_CHIFF				// not tested; see subclasses
//#ifdef CSL_VIBRATO

class SHARCAddInstrument : public Instrument {
public:
	SHARCAddInstrument();			///< Constructor
	SHARCAddInstrument(SHARCInstrument *);

	SHARCAddInstrument(SHARCAddInstrument &);		///< copy constructor
	~SHARCAddInstrument();
								/// Plug functions
	virtual void setParameter(unsigned selector, int argc, void **argv, const char *types);
								/// Play functions
	virtual void playOSC(int argc, void **argv, const char *types);
	
	virtual void playNote(float dur = 1, float ampl = 1, float pitch = 220.0, float pos = 0.5,
				float att = 0.05, float dec = 0.05, float sus = 0.5, float rel = 0.5, float chiff = 0.05);
	virtual void playMIDI(float dur, int chan, int key, int vel);

	ADSR mAEnv;					///< amplitude envelope
#ifdef CSL_VIBRATO
	AR mVEnv;					///< vibrato envelope
	Osc mVib;					///< vibrato oscillator
#endif
#ifdef CSL_CHIFF
	ADSR mCEnv;					///< chiff envelope
	PinkNoise mChiff;			///< attack chiff
	Mixer mMix;					///< output summer
#endif
	SumOfSines mSOS;				///< sum-of-sines oscillator
	Panner mPanner;				///< stereo panner
	SHARCInstrument * mInstr;		///< the SHARC list of spectra

protected:
	virtual void init();			///< set up the instrument's ugen graph and reflective accessors
	void getSpectrum();			///< recompute the SOS spectrum from the SHARC data
	int mNoteFreq = 0;			///< freq of last note
	float mFreq;
};

///
/// SHARCAddInstrumentV - uses the SHARC spectra to create sum-of-sines players.
/// An example of adding features like vibrato in subclasses
///

class SHARCAddInstrumentV : public SHARCAddInstrument {
public:
	SHARCAddInstrumentV();			///< Constructor
	SHARCAddInstrumentV(SHARCInstrument *);

	SHARCAddInstrumentV(SHARCAddInstrument &);		///< copy constructor
	~SHARCAddInstrumentV();
								/// Plug functions
	void setParameter(unsigned selector, int argc, void **argv, const char *types);
								/// Play functions
	void playOSC(int argc, void **argv, const char *types);

	void playNote(float dur = 1, float ampl = 1, float pitch = 220.0, float pos = 0.5,
				float att = 0.05, float dec = 0.05, float sus = 0.5, float rel = 0.5, float chiff = 0.05);
	void playMIDI(float dur, int chan, int key, int vel);

//	ADSR mAEnv;					///< amplitude envelope
	ADSR mVEnv;					///< vibrato envelope
	Osc mVib;					///< vibrato oscillator
//	SumOfSines mSOS;				///< sum-of-sine oscillator
//	Panner mPanner;				///< stereo panner
//	SHARCInstrument * mInstr;		///< the SHARC list of spectra

protected:
	void init();
//	void getSpectrum();			///< recompute the SOS spectrum from the SHARC data
//	int mNoteFreq = 0;			///< freq of last note
//	float mFreq;
};

///
/// VAdditiveInstrument = vector-additive - cross-fade between 2 SOS spectra
///		Envelope mXEnv(dur, pause, xfade, 0.0f) 
///			float paus = fRandV(0.5) * dur;
///			float fade = fRandV(dur - paus);
///		AR(float t, float i, float a, float r);
///

class VAdditiveInstrument : public Instrument {
public:
	VAdditiveInstrument(SHARCSpectrum * spect1, SHARCSpectrum * spect2);
	VAdditiveInstrument(SHARCInstrument * instr1, SHARCInstrument * instr2);
	~VAdditiveInstrument();
								/// Plug functions
	void setParameter(unsigned selector, int argc, void **argv, const char *types);
								/// Play functions
	void playOSC(int argc, void **argv, const char *types);	
	
	void playNote(float dur = 2, float ampl = 1,
				float c_fr = 110, float pos = 0,
				float att = 0.1, float dec = 0.1, float sus = 0.75, float rel = 0.2);
	void playMIDI(float dur, int chan, int key, int vel);	

	SHARCInstrument * mInstr1 = 0;	///< SHARC instruments, or 0 if SHARC spectra are used
	SHARCInstrument * mInstr2 = 0;
	ADSR mAEnv1, mAEnv2;				///< amplitude envelopes
	AR mVEnv;						///< vibrato envelope
	LineSegment mXEnv1, mXEnv2;		///< cross-fade envelopes = line segs
   	SumOfSines mSOS1, mSOS2;			///< 2 sum-of-sine oscillators
	Osc mVib;						///< vibrato oscillator
	Mixer mMix;						///< output summer
	Panner mPanner;					///< stereo panner
protected:
	void init();
	void getSpectra();				///< recompute the 2 SOS spectra from the SHARC data
	int mNoteFreq = 0;			///< freq of last note
	float mFreq;
};

}

#endif
