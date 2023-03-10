//
// FMInstrument.cpp -- Simple FM example instrument class.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
// 
// Implementation of the 2-envelope FM example as an instrument class.
// This example shows how to create an instrument class from a DSP graph and set up its accessors for use with OSC.

#include "FMInstrument.h"

using namespace csl;

#define BASE_FREQ 70.0f
extern bool gVerbose;

#ifdef WET_DRY_MIX
extern Mixer * gIMix;									// stereo instrument mixer
extern Mixer * gOMix;									// stereo output mixer
#endif
// The constructor initializes the DSP graph's UGens

FMInstrument::FMInstrument() :			// initializers for the UGens
		Instrument(),					// inherited constructor
		mAEnv(4.0, 0.10, 0.1, 0.5, 0.5),	// set up 2 standard ADSRs
		mIEnv(4.0, 0.25, 0.3, 0.9, 1.0),
		mMod(BASE_FREQ),					// modulator osc
		mPanner(mCar, 0.0) {				// init the panner
	mIEnv.scaleValues(BASE_FREQ / 2);		// scale the index envelope by the mod. freq
//	mIEnv.setScale(0.5);
	mMod.setScale(mIEnv);
	mMod.setOffset(BASE_FREQ);
	mCar.setFrequency(mMod);
	mCar.setScale(mAEnv);
	mNumChannels = 2;

	mName = "Basic_FM";
	mGraph = & mPanner;						// store the root of the graph as the inst var _graph
	mUGens["Carrier"] = & mCar;				// add ugens that can be monitored to the map
	mUGens["Modulator"] = & mMod;
	mUGens["F env"] = & mIEnv;
	mUGens["A env"] = & mAEnv;
	mUGens["Panner"] = & mPanner;

	mEnvelopes.push_back(& mAEnv);			// list envelopes for retrigger
	mEnvelopes.push_back(& mIEnv);
											// set up accessor vector
	mAccessors.push_back(new Accessor("du", set_duration_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("in", set_index_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("cf", set_c_freq_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("mf", set_m_freq_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("po", set_position_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("aa", set_attack_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("ad", set_decay_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("as", set_sustain_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("ar", set_release_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("ia", set_iattack_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("id", set_idecay_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("is", set_isustain_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("ir", set_irelease_f, CSL_FLOAT_TYPE));
}

// copy constructor

FMInstrument::FMInstrument(FMInstrument& in) :
	Instrument(in),
	mAEnv(in.mAEnv),
	mIEnv(in.mIEnv),
   	mCar(in.mCar),
   	mMod(in.mMod),
	mPanner(in.mPanner) { }

// The destructor frees the stuff we allocated

FMInstrument::~FMInstrument() { }

// Plug function for use by OSC setter methods

void FMInstrument::setParameter(unsigned selector, int argc, void **argv, const char *types) {
	if (argc == 1) {
		float d = * (float *) argv[0];
		if (types[0] == 'i') 
			d = (float) (* (int *) argv[0]);
		switch (selector) {					// switch on which parameter is being set
			case set_duration_f:
				mAEnv.setDuration(d);
				mIEnv.setDuration(d);
				break;
			case set_amplitude_f:
				mAEnv.scaleValues(d); break;
			case set_index_f:
				mIEnv.scaleValues(d); break;
			case set_c_freq_f:
				mMod.setOffset(d); 
	//			printf("\tcf %g\n", * (float *)d);
				break;	
			case set_m_freq_f:
				mMod.setFrequency(d); break;
			case set_position_f:
				mPanner.setPosition(d); 
	//			printf("\tpo %g\n", * (float *)d);
				break;
			case set_attack_f:
				mAEnv.setAttack(d); break;
			case set_decay_f:
				mAEnv.setDecay(d); break;
			case set_sustain_f:
				mAEnv.setSustain(d); break;
			case set_release_f:
				mAEnv.setRelease(d); break;
			case set_iattack_f:
				mIEnv.setAttack(d); break;
			case set_idecay_f:
				mIEnv.setDecay(d); break;
			case set_isustain_f:
				mIEnv.setSustain(d); break;
			case set_irelease_f:
				mIEnv.setRelease(d); break;
			default:
				logMsg(kLogError, "Unknown selector in FMInstrument set_parameter selector: %d\n", selector);
		 }
	} else {			// multiple args
		logMsg(kLogError, "Unknown multi-arg (%d) setter in FMInstrument: %s\n", argc, types);
	}
}

// Formats (4, 6 or 14 arguments):
// 	dur, ampl, fr, pos
// 	dur, ampl, c_fr, m_fr, ind, pos
// 	dur, ampl, c_fr, m_fr, ind, pos, att, dec, sus, rel, i_att, i_dec, i_sus, i_rel
// Example
// 				 dur,  ampl, c_fr,   m_fr,   ind,    pos,  att,   dec,   sus,  rel,  i_att, i_dec, i_sus, i_rel
// OSC: /i37/pn  0.323 0.499 204.123 257.099 193.352 0.120 0.0431 0.0426 0.560 0.119 0.070  0.005  1.0    0.1

void FMInstrument::parseArgs(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	unsigned nargs;
	if (strcmp(types, "ffff") == 0) {
		nargs = 4;
		if (gVerbose)
			printf("\tFM:  d %5.2f   a %5.2f   f %7.1f   p %5.2f\n",
				*fargs[0], *fargs[1], *fargs[2], *fargs[3]);
	} else if (strcmp(types, "ffffff") == 0) {
		nargs = 6;
		if (gVerbose)
			printf("\tFM: d %g   a %g c %g m %g i %g   p %g\n",
				*fargs[0], *fargs[1], *fargs[2], *fargs[3], *fargs[4], *fargs[5]);
	} else if (strcmp(types, "ffffffffffffff") == 0) {
		nargs = 14;
		if (gVerbose)
			printf("\tFM: d %.3f a %.3f p %.3f i %.3f - a %.3f d %.3f s %.3f r %.3f - a %.3f d %.3f s %.3f r %.3f\n",
				   *fargs[0], *fargs[1], *fargs[2], *fargs[4], *fargs[6], *fargs[7], *fargs[8], *fargs[9],
				   *fargs[10], *fargs[11], *fargs[12], *fargs[13]);
	} else {
		logMsg(kLogError, "Invalid type string in OSC message, expected \"ff...ff\" got \"%s\"\n", types);
		mAEnv.scaleValues(0.0f);
		return;
	}
	mAEnv.setDuration(*fargs[0]);
	mIEnv.setDuration(*fargs[0]);
	mAEnv.scaleValues(*fargs[1]);
	mMod.setOffset(*fargs[2]);
	mMod.setFrequency(*fargs[2]);

	if (nargs == 4) {
		mIEnv.setScale(*fargs[2]);
		mPanner.setPosition(*fargs[3]);
	}
	if (nargs == 6) {
		mIEnv.setScale(*fargs[4] * *fargs[2]);
		mPanner.setPosition(*fargs[5]);
	}
	if (nargs == 14) {
		mMod.setFrequency(*fargs[3]);
		mIEnv.setScale(*fargs[4] * *fargs[2]);
		mPanner.setPosition(*fargs[5]);
		mAEnv.setAttack(*fargs[6]);
		mAEnv.setDecay(*fargs[7]);
		mAEnv.setSustain(*fargs[8]);
		mAEnv.setRelease(*fargs[9]);
		mIEnv.setAttack(*fargs[10]);
		mIEnv.setDecay(*fargs[11]);
		mIEnv.setSustain(*fargs[12]);
		mIEnv.setRelease(*fargs[13]);
	}
}

// Play a note with a given arg list

void FMInstrument::playOSC(int argc, void **argv, const char *types) {
	this->parseArgs(argc, argv, types);
	this->play();
}

// Play a note specifying all parameters

void FMInstrument::playNote(float dur, float ampl, float c_fr, float m_fr, float ind, float pos, 
				float att, float dec, float sus, float rel, 
				float i_att, float i_dec, float i_sus, float i_rel) {
	mAEnv.setDuration(dur);
	mIEnv.setDuration(dur);
	mAEnv.scaleValues(ampl);
	mMod.setOffset(c_fr);
	mMod.setFrequency(m_fr);
	mIEnv.scaleValues(ind * m_fr);
	mPanner.setPosition(pos);
	mAEnv.setAttack(att);
	mAEnv.setDecay(dec);
	mAEnv.setSustain(sus);
	mAEnv.setRelease(rel);
	mIEnv.setAttack(i_att);
	mIEnv.setDecay(i_dec);
	mIEnv.setSustain(i_sus);
	mIEnv.setRelease(i_rel);
	this->play();
}

void FMInstrument::playMIDI(float dur, int chan, int key, int vel) {
	mAEnv.setDuration(dur);
	mIEnv.setDuration(dur);
	mAEnv.scaleValues(sqrtf((float) vel / 128.0f));
	mMod.setOffset(keyToFreq(key));
	mMod.setFrequency(keyToFreq(key));
//	mIEnv.scaleValues(keyToFreq(key));
	this->play();
}

///////////////////////////////////////////////////

#pragma mark FancyFMInstrument

// FancyFMInstrument

//	Envelope mVibEnv;					///< vibrato envelope
//	ADSR mChiffEnv;					///< attack-chiff envelope
//  Sine mVibrato;					///< sine oscillator for vibrato
//	WhiteNoise mChiff;				///< chiff noise
//	Butter mChFilter;					///< chiff filter
//	Freeverb mReverb;

FancyFMInstrument::FancyFMInstrument() :		// initializers for the UGens
			FMInstrument(),
			mVibEnv(kExpon, 3, 0.0, 0.1, 2, 1.0, 3, 0.1),	///< Triangle-like AR vibrato envelope
			mChiffEnv(3, 0.01, 0.01, 0.0, 2),				///< attack-chiff envelope
			mVibrato(6),									///< sine oscillator for vibrato
			mChFilter(mChiff, BW_BAND_PASS, 4000.f, 200.f),	///< chiff filter
			mReverb(mPanner) {					// reverb the panner
	mVibEnv.setScale(8);							// scale vibrato envelope
	mVibrato.setScale(mVibEnv);
	mVibrato.setOffset(BASE_FREQ);				// shift vibrato up

	mChiffEnv.setScale(0.8);						// scale chiff envelope
	mChFilter.setScale(mChiffEnv);				// apply chiff envelope
	mCar.setOffset(mChFilter);					// add in chiff

	mMod.setOffset(mVibrato);						// add in vibrato and base freq

	mName = "Fancy_FM";
	mUGens["Vibrato"] = & mVibrato;
	mUGens["Vib env"] = & mVibEnv;
	mUGens["Attack chiff filter"] = & mChFilter;
	mUGens["Attack chiff env"] = & mChiffEnv;
	mUGens["Attack noise"] = & mChiff;
	mUGens["Reverb"] = & mReverb;
	mEnvelopes.push_back(& mVibEnv);				// list envelopes for retrigger
	mEnvelopes.push_back(& mChiffEnv);
}

// The destructor frees the stuff we allocated

FancyFMInstrument::~FancyFMInstrument() { }

void FancyFMInstrument::setParameter(unsigned selector, int argc, void **argv, const char *types) {
	FMInstrument::setParameter(selector,  argc, argv, types);				// call superclass' version
	if (argc == 1) {
		float d = * (float *) argv[0];
		if (types[0] == 'i') 
			d = (float) (* (int *) argv[0]);
		switch (selector) {						// switch on which parameter is being set
			case set_duration_f:
				mChiffEnv.setDuration(d);
				mVibEnv.setDuration(d);
				break;
			case set_c_freq_f:
				mVibrato.setOffset(d);			// shift vibrato up
				mMod.setOffset(mVibrato);
				break;
			case set_vib_depth_f:
				mVibEnv.setScale(d); break;
			case set_chiff_amt_f:
				mChiffEnv.setScale(d); break;
			default:
				break;
		 }
	} else 			// multiple args
		logMsg(kLogError, "Unknown multi-arg (%d) setter in FMInstrument: %s\n", argc, types);
}

void FancyFMInstrument::playOSC(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	this->parseArgs(argc, argv, types);
	mVibrato.setOffset(*fargs[2]);			// shift vibrato up
	mMod.setOffset(mVibrato);					// add in vibrato and base freq
	mMod.setOffset(*fargs[2]);
	mVibrato.setFrequency(4.0 + (4.0 * fRandZ()));
	mChiffEnv.setAttack(0.05 * fRandZ());
	mChiffEnv.setDecay(0.05 * fRandZ());
	this->play();
}


void FancyFMInstrument::playMIDI(float dur, int chan, int key, int vel) {
	mAEnv.setDuration(dur);
	mIEnv.setDuration(dur);
	mAEnv.scaleValues(sqrtf((float) vel / 128.0f));
	mVibrato.setOffset(keyToFreq(key));		// shift vibrato up
	mMod.setOffset(mVibrato);					// add in vibrato and base freq
	mMod.setOffset(keyToFreq(key));
	mVibrato.setFrequency(4.0 + (4.0 * fRandZ()));
	mChiffEnv.setAttack(0.05 * fRandZ());
	mChiffEnv.setDecay(0.05 * fRandZ());
	this->play();
}

///////////////////////////////////////////////////////////////////////////////////////////////

#pragma mark FMBell

///< Constructor

FMBell::FMBell() :						// initializers for the UGens
		Instrument(),					// inherited constructor
		mAEnv(4.0f, 0.5f, 0.00001f, kExpon),// set up 3 exp-segs
		mIEnv(4.0f, 100.0f, 1.0f, kExpon),
		mGliss(4.0f, BASE_FREQ, BASE_FREQ*0.9f, kExpon),
		mPanner(mCar, 0.0) {				// init the panner
	mNumChannels = 2;
	carFr = BASE_FREQ;
	glRatio = 0.9;
	modRatio = BASE_FREQ;

	mMod.setFrequency(BASE_FREQ);			// plug together envs and oscs
	mMod.setScale(mIEnv);
	mMod.setOffset(mGliss);
	mCar.setFrequency(mMod);
	mCar.setScale(mAEnv);

	mName = "FM_Bell";
	mGraph = & mPanner;					// store the root of the graph as the inst var _graph
	mUGens["Carrier"] = & mCar;			// add ugens that can be monitored to the map
	mUGens["Modulator"] = & mMod;
	mUGens["A env"] = & mAEnv;
	mUGens["Panner"] = & mPanner;
	mUGens["Gliss"] = & mPanner;

	mEnvelopes.push_back(& mAEnv);		// list envelopes for retrigger
	mEnvelopes.push_back(& mIEnv);
	mEnvelopes.push_back(& mGliss);
										// set up accessor vector
	mAccessors.push_back(new Accessor("du", set_duration_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("cf", set_c_freq_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("fr", set_cm_freq_r, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("gl", set_gliss_r, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("po", set_position_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("wd", wet_dry_f, CSL_FLOAT_TYPE));
			
}

FMBell::FMBell(FMBell& in)  :
		Instrument(in),
		mAEnv(in.mAEnv),
		mIEnv(in.mIEnv),
		mGliss(in.mGliss),
		mCar(in.mCar),
		mMod(in.mMod),
		mPanner(in.mPanner)
	{ }

FMBell::~FMBell() { }
	
void FMBell::setParameter(unsigned selector, int argc, void **argv, const char *types) {
	if (argc == 1) {
		float d = * (float *) argv[0];
		if (types[0] == 'i')
			d = (float) (* (int *) argv[0]);
		switch (selector) {					// switch on which parameter is being set
			case set_duration_f:
				mAEnv.setDuration(d);
				mIEnv.setDuration(d);
				mGliss.setDuration(d);
				break;
			case set_amplitude_f:
				mAEnv.setStart(d); break;
			case set_index_f:
				mInd = d * carFr;
				mIEnv.setStart(mInd); break;
			case set_c_freq_f:
				carFr = d;
				mGliss.setStart(carFr);
				mGliss.setEnd(glRatio * carFr);
				mMod.setFrequency(modRatio * carFr); break;
				break;
			case set_cm_freq_r:
				modRatio = d;
				mMod.setFrequency(modRatio * carFr); break;
			case set_gliss_r:
				glRatio = d;
				mGliss.setStart(carFr);
				mGliss.setEnd(glRatio * carFr); break;
				break;
			case set_position_f:
				mPanner.setPosition(d);
				break;
#ifdef WET_DRY_MIX
			case wet_dry_f:
				gIMix->scaleInput(*this, 1.0f - d);
				gOMix->scaleInput(*this, d);
				break;
#endif
			default:
				logMsg(kLogError, "Unknown selector in FMInstrument set_parameter selector: %d\n", selector);
		}
	} else {			// multiple args
		logMsg(kLogError, "Unknown multi-arg (%d) setter in FMInstrument: %s\n", argc, types);
	}
}

// Example
// 				 dur, ampl, fr0,  gliss, rat,  ind,  pos    wet/dry
// OSC: /i37/pn  4.0  0.49  204.1 1.0    1.933 2.0   0.0    1.0
//				 0    1     2     3      4     5     6      7

void FMBell::parseArgs(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
#ifdef WET_DRY_MIX
	if (strcmp(types, "ffffffff") == 0)
		if (gVerbose)
			printf("\tFM_Bell: d %.3f a %.3f f %.3f g %.3f r %.3f i %.3f p %.3f w %.3f\n",
				   *fargs[0], *fargs[1], *fargs[2], *fargs[3], *fargs[4], *fargs[5], *fargs[6], *fargs[7]);
	if (strcmp(types, "ffffffff") != 0) {
		logMsg(kLogError, "Invalid type string in OSC message, expected \"ffffffff\" got \"%s\"\n", types);
		mAEnv.setStart(0.0f);
		return;
	}
#else
	if (strcmp(types, "fffffff") == 0)
		if (gVerbose)
			printf("\tFM_Bell: d %.3f a %.3f f %.3f g %.3f r %.3f i %.3f p %.3f\n",
				   *fargs[0], *fargs[1], *fargs[2], *fargs[3], *fargs[4], *fargs[5], *fargs[6]);
	if (strcmp(types, "fffffff") != 0) {
		logMsg(kLogError, "Invalid type string in OSC message, expected \"fffffff\" got \"%s\"\n", types);
		mAEnv.setStart(0.0f);
		return;
	}
#endif
	mAEnv.setDuration(*fargs[0]);
	mIEnv.setDuration(*fargs[0]);
	mGliss.setDuration(*fargs[0]);
	mAEnv.setStart(*fargs[1]);
	mGliss.setStart(*fargs[2]);
	mGliss.setEnd(*fargs[2] * *fargs[3]);
	mIEnv.setStart(*fargs[5] * *fargs[2]);
	mIEnv.setEnd(*fargs[2]);
	mMod.setFrequency(*fargs[2] * *fargs[4]);
	mPanner.setPosition(*fargs[6]);
#ifdef WET_DRY_MIX
	float wet = *fargs[7];
	float wetS = sqrtf(wet);			// square-root it for a more equal-loudness cross-fade
	gOMix->scaleInput(*this, 1.0f - wetS);
	gIMix->scaleInput(*this, wetS);
#endif
}

void FMBell::playOSC(int argc, void **argv, const char *types) {
	this->parseArgs(argc, argv, types);
	this->play();
}

void FMBell::playNote(float dur, float ampl, float fr, float gliss, float rat, float ind, float pos, float direct) {
	mAEnv.setDuration(dur);
	mIEnv.setDuration(dur);
	mGliss.setDuration(dur);
	mAEnv.setStart(ampl);
	mGliss.setStart(fr);
	mGliss.setEnd(fr * rat);
	mMod.setOffset(fr);
	mMod.setFrequency(fr);
	mIEnv.setStart(ind * fr);
	mPanner.setPosition(pos);
#ifdef WET_DRY_MIX
	gIMix->scaleInput(*this, 1.0f - direct);
	gOMix->scaleInput(*this, direct);
#endif
	this->play();
}

void FMBell::playMIDI(float dur, int chan, int key, int vel) {
	mAEnv.setDuration(dur);
	mIEnv.setDuration(dur);
	mAEnv.setStart(sqrtf((float) vel / 128.0f));
	mMod.setOffset(keyToFreq(key));
	mMod.setFrequency(keyToFreq(key));
	this->play();
}

void FMBell::dump() {
	printf("\tFM_Bell: d %.3f a %.3f f %.3f g %.3f r %.3f i %.3f\n",
		   mAEnv.duration(), mAEnv.start(), mGliss.start(), glRatio, mInd);
}
