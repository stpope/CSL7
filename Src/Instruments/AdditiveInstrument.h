///
/// AdditiveInstrument.h -- Sum-of-sines synthesis instrument classes
/// There are several simple and more-sophisticated additive instruments here.
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
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
/// AdditiveInstrument - the simplest instruments using a computed som-of-sines oscillator.
/// Use this to construct arbitrary spectra with the c'tors that take SumOfSines as arguments.
/// Subclasses do more interesting things...
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
/// An example of adding features like attack chiff and vibrato with conditional compilation
/// (messy, I know; see the subclasses below for versions that work).
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
/// SHARCAddInstrumentV - uses the SHARC spectra to create sum-of-sines players with vibrato.
/// ToDo: add OSC arguments for vibrato rate and depth.
///

class SHARCAddInstrumentV : public SHARCAddInstrument {
public:
	SHARCAddInstrumentV();			///< Constructor
	SHARCAddInstrumentV(SHARCInstrument *);

	~SHARCAddInstrumentV();
								/// Plug functions
	void setParameter(unsigned selector, int argc, void **argv, const char *types);
								/// Play functions
	void playOSC(int argc, void **argv, const char *types);

	void playNote(float dur = 1, float ampl = 1, float pitch = 220.0, float pos = 0.5,
				float att = 0.05, float dec = 0.05, float sus = 0.5, float rel = 0.5, float chiff = 0.05);
	void playMIDI(float dur, int chan, int key, int vel);

	ADSR mVEnv;					///< vibrato envelope
	Osc mVib;					///< vibrato oscillator

protected:
	void init();
};

///
/// SHARCAddInstrumentC - uses the SHARC spectra to create sum-of-sines players with attack chiff.
///

class SHARCAddInstrumentC : public SHARCAddInstrument {
public:
	SHARCAddInstrumentC();			///< Constructor
	SHARCAddInstrumentC(SHARCInstrument *);

	SHARCAddInstrumentC(SHARCAddInstrumentC &);		///< copy constructor
	~SHARCAddInstrumentC();
								/// Plug functions
	void setParameter(unsigned selector, int argc, void **argv, const char *types);
								/// Play functions
	void playOSC(int argc, void **argv, const char *types);

	ADSR mCEnv;					///< chiff envelope
	PinkNoise mChiff;			///< attack chiff
	Mixer mMix;					///< output summer

protected:
	void init();
};

///
/// VAdditiveInstrument = vector-additive - cross-fade between 2 SOS spectra.
/// This is an example of a lower-level DSP graph (see c'tor implementations) that use
/// explicit add/mul operators rather than the setScale() and setOffset() functions
/// (MusicV- and cmusic-style) (no real difference in performance).
///

class VAdditiveInstrument : public Instrument {
public:
	VAdditiveInstrument(SHARCSpectrum * spect1, SHARCSpectrum * spect2);
	VAdditiveInstrument(SHARCInstrument * instr1, SHARCInstrument * instr2);
	~VAdditiveInstrument();
								/// Plug functions
	virtual void setParameter(unsigned selector, int argc, void **argv, const char *types);
								/// Play functions
	virtual void playOSC(int argc, void **argv, const char *types);
	
	virtual void playNote(float dur = 2, float ampl = 1,
				float c_fr = 110, float pos = 0,
				float att = 0.1, float dec = 0.1, float sus = 0.75, float rel = 0.2);
	virtual void playMIDI(float dur, int chan, int key, int vel);

	SHARCInstrument * mInstr1 = 0;	///< SHARC instruments, or 0 if SHARC spectra are used
	SHARCInstrument * mInstr2 = 0;
	ADSR mAEnv;						///< amplitude envelopes
	ADSR mVEnv;						///< vibrato envelope
	Osc mVib;						///< vibrato oscillator
	LineSegment mXEnv1, mXEnv2;		///< cross-fade envelopes = line segs
   	SumOfSines mSOS1, mSOS2;			///< 2 sum-of-sine oscillators
	MulOp mEMul1, mEMul2;			///< fade-in/out scalers
	AddOp mVibAdd, mSummer;			///< summer of 2 oscs
	MulOp mAMul;						///< final env multiplier
	Panner mPanner;					///< stereo panner
protected:
	virtual void init();
	void getSpectra();				///< recompute the 2 SOS spectra from the SHARC data
	int mNoteFreq = 0;				///< freq of last note
	float mFreq;
};

///
/// VAdditiveInstrumentR = vector-additive cross-fade between 2 SOS spectra
/// with random-walk cross-fade envelope
///

class VAdditiveInstrumentR : public VAdditiveInstrument {
public:
	VAdditiveInstrumentR(SHARCSpectrum * spect1, SHARCSpectrum * spect2);
	VAdditiveInstrumentR(SHARCInstrument * instr1, SHARCInstrument * instr2);
	~VAdditiveInstrumentR();
								/// Plug functions
	void setParameter(unsigned selector, int argc, void **argv, const char *types);
								/// Play functions
	void playOSC(int argc, void **argv, const char *types);

	RandEnvelope mREnv;					///< random cross-fade envelope
	SubOp mESub;							///< SubOp for (1 - env) cross-fade
protected:
	void init();
};

} // namespace

#endif

