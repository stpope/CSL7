///
/// AdditiveInstrument.h -- Sum-of-sines synthesis instrument class.
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// 

/// Accessors
///		"du", set_duration_f
///		"am", set_amplitude_f
///		"fr", set_frequency_f
///		"po", set_position_f
///		"at", set_attack_f	-- amplitude envelope ADSR
///		"de", set_decay_f
///		"su", set_sustain_f
///		"re", set_release_f
///		"pt", set_partial_f
///		"ps", set_partials_f
///
/// OSC note formats (4 or 8 arguments):
/// 	dur, ampl, c_fr, pos
/// 	dur, ampl, c_fr, pos, att, dec, sus, rel

#include "AdditiveInstrument.h"

extern bool gVerbose;

using namespace csl;

// The constructor initializes the DSP graph's UGens

AdditiveInstrument::AdditiveInstrument() :			// initializers for the UGens
		Instrument(),
		mAEnv(0.25, 0.03, 0.06, 0.5, 0.15),		// set up standard ADSR (1/4-sec duration)
		mSOS(16, 0.75),							// SumOfSines init, 16 partials @ 1/f
		mPanner(mSOS, 0.0) {						// init the panner
	this->init();
}

AdditiveInstrument::AdditiveInstrument(SumOfSines & sos) :	// initializers for the UGens
		Instrument(),
		mAEnv(0.25, 0.03, 0.06, 0.5, 0.15),		// set up standard ADSR (1/4-sec duration)
		mSOS(sos),								// SumOfSines init, 16 partials @ 1/f
		mPanner(mSOS, 0.0) {						// init the panner
	this->init();
}

AdditiveInstrument::AdditiveInstrument(unsigned numHarms, float noise) :	// initializers for the UGens
		Instrument(),
		mAEnv(0.25, 0.03, 0.06, 0.5, 0.15),		// set up standard ADSR (1/4-sec duration)
		mSOS(numHarms, noise),					// SumOfSines init, numHarms partials @ 1/f
		mPanner(mSOS, 0.0) {						// init the panner
	this->init();
}

AdditiveInstrument::AdditiveInstrument(SHARCSpectrum & spect) :
		Instrument(),
		mAEnv(0.25, 0.03, 0.06, 0.5, 0.15),		// set up standard ADSR (1/4-sec duration)
		mSOS(spect),								// SumOfSines init, 16 partials @ 1/f
		mPanner(mSOS, 0.0) {						// init the panner
	this->init();
}

// setup instance

void AdditiveInstrument::init() {
	mSOS.setScale(mAEnv);							// plug in the envelope
	mNumChannels = 2;
	mName = "SumOfSines";							// set graph's name
	mGraph = & mPanner;							// store the root of the graph as the inst var _graph
	mUGens["Osc"] = & mSOS;						// add ugens that can be monitored to the map
	mUGens["A env"] = & mAEnv;
	mUGens["Pos"] = & mPanner;
	mEnvelopes.push_back(& mAEnv);				// list envelopes for retrigger
												// set up accessor vector
	mAccessors.push_back(new Accessor("du", set_duration_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("fr", set_frequency_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("po", set_position_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("at", set_attack_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("de", set_decay_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("su", set_sustain_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("re", set_release_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("pt", set_partial_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("ps", set_partials_f, CSL_FLOAT_TYPE));
}

// copy constructor

AdditiveInstrument::AdditiveInstrument(AdditiveInstrument& in) :
	Instrument(in),
	mAEnv(in.mAEnv),
   	mSOS(in.mSOS),
	mPanner(in.mPanner) { }

// The destructor frees the stuff we allocated

AdditiveInstrument::~AdditiveInstrument() { }

// Plug function for use by OSC setter methods

void AdditiveInstrument::setParameter(unsigned selector, int argc, void **argv, const char *types) {
	if (argc == 1) {
		float d = * (float *) argv[0];
		if (types[0] == 'i') 
			d = (float) (* (int *) argv[0]);
		float s;
		if (types[0] == 'i') 
			d = (float) (* (int *) argv[0]);
		if (argc > 1) {
			s = * (float *) argv[1];
			if (types[1] == 'i') 
				s = (float) (* (int *) argv[1]);
		}
//		logMsg("Set %d SOS params to (%s)\n", argc, types);
		switch (selector) {					// switch on which parameter is being set
			case set_duration_f:
				mAEnv.setDuration(d);		break;
			case set_amplitude_f:
				mAEnv.setScale(d);			break;
			case set_frequency_f:
				mSOS.setFrequency(d); 		break;	
			case set_position_f:
				mPanner.setPosition(d); 	break;
			case set_attack_f:
				mAEnv.setAttack(d);			break;
			case set_decay_f:
				mAEnv.setDecay(d);			break;
			case set_sustain_f:
				mAEnv.setSustain(d);		break;
			case set_release_f:
				mAEnv.setRelease(d);		break;
			case set_partial_f:
				mSOS.addPartial(d, s);		break;
			default:
				logMsg(kLogError, "Unknown selector in AdditiveInstrument set_parameter selector: %d\n", selector);
		 }
	} else {			// multiple args
		if (selector == set_partials_f) {
			logMsg("Set %d SOS partials to (%s)\n", argc, types);
			mSOS.addPartials(argc, argv); 
		} else {
			logMsg(kLogError, "Unknown multi-arg (%d) setter in AdditiveInstrument: %s\n", argc, types);
		}
	}
}

/// OSC note formats (4 or 8 arguments):
/// 	dur, ampl, c_fr, pos
/// 	dur, ampl, c_fr, pos, att, dec, sus, rel

void AdditiveInstrument::playOSC(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	unsigned nargs;

//	if (strcmp(types, "ffff") == 0)
	if (argc == 4)
		nargs = 4;
//	else if (strcmp(types, "ffffffff") == 0)
	else if (argc == 8)
		nargs = 8;
	else {
		logMsg(kLogError, "Invalid type string in OSC message, expected \"ff...ff\" got \"%s\"\n", types);
		return;
	}
	if (gVerbose)
		printf("\tAdd: d %5.2f   a %5.2f   f %7.1f   p %5.2f\n", *fargs[0], *fargs[1], *fargs[2], *fargs[3]);
	mAEnv.setDuration(*fargs[0]);
	mAEnv.scaleValues(*fargs[1]);
	mSOS.setFrequency(*fargs[2]);
	mPanner.setPosition(*fargs[3]);
	if (nargs == 8) {
		printf("\t\ta %5.2f d %5.2f s %5.2f r %5.2f\n", fargs[4], fargs[5], fargs[6], fargs[7]);
		mAEnv.setAttack(*fargs[4]);
		mAEnv.setDecay(*fargs[5]);
		mAEnv.setSustain(*fargs[6]);
		mAEnv.setRelease(*fargs[7]);
	}
	this->play();
}

// Play a note specifying all parameters

void AdditiveInstrument::playNote(float dur, float ampl, float c_fr, float pos, float att, float dec, float sus, float rel) {
	mAEnv.setDuration(dur);
	mAEnv.scaleValues(ampl);
	mSOS.setFrequency(c_fr);
	mPanner.setPosition(pos);
	mAEnv.setAttack(att);
	mAEnv.setDecay(dec);
	mAEnv.setSustain(sus);
	mAEnv.setRelease(rel);
	this->play();
}


void AdditiveInstrument::playMIDI(float dur, int chan, int key, int vel) {
	mAEnv.setDuration(dur);
	mAEnv.scaleValues(sqrtf((float) vel / 128.0f));
	mSOS.setFrequency(keyToFreq(key));
	this->play();
}

#pragma mark SHARCAddInstrument // ------------------------------------------------------

/// recompute the SOS spectrum from the SHARC data

void csl::getSHARCSpectrum(SHARCInstrument * inst, float freq, SumOfSines * sos) {
	sos->clearPartials();
	SHARCSpectrum * spect = inst->spectrum_with_frequency(freq);
	for (unsigned i = 0; i < spect->_num_partials; i++) {
		Partial * harm = spect->_partials[i];
		sos->addPartial(harm->number, harm->amplitude, harm->phase);
	}
	sos->createCache();							// make the cached wavetable
//	sos->mWavetable.dumpSamples(0, 10, 100);		// print some buffer values for debugging
}


// Default constructor (unused)

SHARCAddInstrument::SHARCAddInstrument	() :
		Instrument(),
		mAEnv(0.5, 0.05, 0.06, 0.5, 0.15),		// set up standard ADSR (1/2-sec duration)
		mPanner(mSOS, 0.0),
		mSOS(160.1f) {							// SumOfSines init -- 1/f spectrum + noise
	this->init();
}

// c'tor with SHARCInstrument = the normal way to use this

SHARCAddInstrument::SHARCAddInstrument(SHARCInstrument * in) :
		Instrument(),
		mAEnv(0.5, 0.06, 0.06, 0.5, 0.1),			// ampl envelope
		mSOS(100.0f),							// SumOfSines init -- 1/f spectrum + noise
		mPanner(mSOS, 0.0),						// stereo panner
		mInstr(in) {
	this->init();								// call init fcn.
}

// copy c'tor

SHARCAddInstrument::SHARCAddInstrument(SHARCAddInstrument& in) :
		Instrument(in),
		mAEnv(in.mAEnv),
		mSOS(in.mSOS),
		mPanner(in.mPanner),
		mInstr(in.mInstr) {
	this->init();
}

SHARCAddInstrument::~SHARCAddInstrument() { }

// init() = setup instance

void SHARCAddInstrument::init() {
	mNumChannels = 2;
//	mSOS.setFrequency(110.0);							// temp freq
	mSOS.setScale(mAEnv);
	mName = "SHARC";									// set graph's name
	mGraph = & mPanner;								// store the root of the graph as the inst var _graph
	mUGens["Osc"] = & mSOS;							// add ugens that can be monitored to the map
	mUGens["A env"] = & mAEnv;
	mUGens["Pos"] = & mPanner;
	
	mEnvelopes.push_back(& mAEnv);					// list envelopes for retrigger
													// set up accessor vector
	mAccessors.push_back(new Accessor("du", set_duration_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("fr", set_frequency_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("po", set_position_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("at", set_attack_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("de", set_decay_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("su", set_sustain_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("re", set_release_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("ch", set_chiff_f, CSL_FLOAT_TYPE));
}

/// Plug functions

void SHARCAddInstrument::setParameter(unsigned selector, int argc, void **argv, const char *types) {
	if (argc == 1) {
		float d = * (float *) argv[0];
		if (types[0] == 'i')
			d = (float) (* (int *) argv[0]);
		float s;
		if (types[0] == 'i')
			d = (float) (* (int *) argv[0]);
		if (argc > 1) {
			s = * (float *) argv[1];
			if (types[1] == 'i')
				s = (float) (* (int *) argv[1]);
		}
//		logMsg("Set %d SOS params to (%s)\n", argc, types);
		switch (selector) {					// switch on which parameter is being set
			case set_duration_f:
				mAEnv.setDuration(d);		break;
			case set_amplitude_f:
				mAEnv.setScale(d);		break;
			case set_frequency_f:
				mSOS.setFrequency(d);
				mFreq = d;				break;
			case set_position_f:
				mPanner.setPosition(d); 	break;
			case set_attack_f:
				mAEnv.setAttack(d);		break;
			case set_decay_f:
				mAEnv.setDecay(d);		break;
			case set_sustain_f:
				mAEnv.setSustain(d);		break;
			case set_release_f:
				mAEnv.setRelease(d);		break;
#ifdef CSL_CHIFF
			case set_chiff_f:
				mCEnv.setScale(d);		break;
#endif
			default:
				logMsg(kLogError, "Unknown selector in SHARCAddInstrument set_parameter selector: %d\n", selector);
		 }
	} else  {
		logMsg(kLogError, "Unknown multi-arg (%d) setter in SHARCAddInstrument: %s\n", argc, types);
	}
}

// Play functions
// ffff = dur, amp, pitch, pos
// fffffffff = dur, amp, pitch, pos, att, dec, sus, rel

void SHARCAddInstrument::playOSC(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	unsigned nargs;

//	if (strcmp(types, "ffff") == 0)
	if (argc == 4)
		nargs = 4;
//	else if (strcmp(types, "ffffffff") == 0)
	else if (argc == 8)
		nargs = 8;
	else {
		logMsg(kLogError, "Invalid type string in OSC message, expected 4 or 9 args got \"%s\"\n", types);
		return;
	}
	if (gVerbose)
		printf("\tSHARC0: d %5.2f   a %5.2f   f %7.1f   p %5.2f\n", *fargs[0], *fargs[1], *fargs[2], *fargs[3]);
	mAEnv.setDuration(*fargs[0]);			// dur
	mAEnv.scaleValues(*fargs[1]);			// amp
	mFreq = *fargs[2];
	mSOS.setFrequency(mFreq);				// freq
	mPanner.setPosition(*fargs[3]);		// pos
	if (nargs == 8) {
//		printf("\t\ta %5.2f d %5.2f s %5.2f r %5.2f\n", fargs[4], fargs[5], fargs[6], fargs[7]);
		mAEnv.setAttack(*fargs[4]);
		mAEnv.setDecay(*fargs[5]);
		mAEnv.setSustain(*fargs[6]);
		mAEnv.setRelease(*fargs[7]);
	}
	if (mNoteFreq != mFreq) {
		getSHARCSpectrum(mInstr, mFreq, & mSOS);
		mNoteFreq = mFreq;
	}
	this->play();
}
	
void SHARCAddInstrument::playNote(float dur, float ampl, float pitch, float pos,
				float att, float dec, float sus, float rel, float chiff) {
	mAEnv.setDuration(dur);
	mAEnv.scaleValues(ampl);
	mSOS.setFrequency(pitch);
	mFreq = pitch;
	mPanner.setPosition(pos);
	mAEnv.setAttack(att);
	mAEnv.setDecay(dec);
	mAEnv.setSustain(sus);
	mAEnv.setRelease(rel);
	if (mNoteFreq != mFreq) {
		getSHARCSpectrum(mInstr, mFreq, & mSOS);
		mNoteFreq = mFreq;
	}
	this->play();
}

void SHARCAddInstrument::playMIDI(float dur, int chan, int key, int vel) {
	mAEnv.setDuration(dur);
	mAEnv.scaleValues(sqrtf((float) vel / 128.0f));
	float frq = keyToFreq(key);
	mSOS.setFrequency(frq);
	mFreq = frq;
	if (mNoteFreq != mFreq) {
		getSHARCSpectrum(mInstr, mFreq, & mSOS);
		mNoteFreq = mFreq;
	}
	this->play();
}

#pragma mark SHARCAddInstrumentV // ------------------------------------------------------

// Default constructor (unused)

SHARCAddInstrumentV::SHARCAddInstrumentV() :
		Instrument(),
		mAEnv(0.5, 0.05, 0.06, 0.5, 0.15),		// set up standard ADSR (1/2-sec duration)
		mPanner(mSOS, 0.0),
		mVEnv(2.0f, 0.5f, 1.0f, 0.1f, 1.0f, 0.1f),
		mSOS(160.1f) {							// vibrato env - IADSR - initial delay and slow attack
	this->init();
}

// c'tor with SHARCInstrument = the normal way to use this

SHARCAddInstrumentV::SHARCAddInstrumentV(SHARCInstrument * in) :
		Instrument(),
		mAEnv(0.5, 0.04, 0.04, 0.5, 0.3),			// ampl envelope
		mVEnv(kSquare, 2.0f, 0.5, 1.0f, 0.1f, 1.0f, 0.3f),	// vibrato env - slow attack
		mSOS(160.1f),
		mPanner(mSOS, 0.0),						// stereo panner
		mInstr(in) {
	this->init();								// call init fcn.
}

SHARCAddInstrumentV::~SHARCAddInstrumentV() { }

// init() = setup instance

void SHARCAddInstrumentV::init() {
	mNumChannels = 2;
	float vibDepth = fRandM(2.0f, 4.0f);
	mVEnv.scaleValues(vibDepth);
	mVib.setFrequency(fRandM(4.0f, 10.0f));			// vibrato freq
	mVib.setOffset(110.0f);
	mVib.setScale(mVEnv);
	mSOS.setScale(mAEnv);
	mSOS.setFrequency(mVib);							// plug in the vibrato

	mName = "SHARC";									// set graph's name
	mGraph = & mPanner;								// store the root of the graph as the inst var _graph
	mUGens["Osc"] = & mSOS;							// add ugens that can be monitored to the map
	mUGens["A env"] = & mAEnv;
	mUGens["Pos"] = & mPanner;

	mEnvelopes.push_back(& mAEnv);					// list envelopes for retrigger
	mEnvelopes.push_back(& mVEnv);
													// set up accessor vector
	mAccessors.push_back(new Accessor("du", set_duration_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("fr", set_frequency_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("po", set_position_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("at", set_attack_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("de", set_decay_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("su", set_sustain_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("re", set_release_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("ch", set_chiff_f, CSL_FLOAT_TYPE));

}

/// Plug functions

void SHARCAddInstrumentV::setParameter(unsigned selector, int argc, void **argv, const char *types) {
	if (argc == 1) {
		float d = * (float *) argv[0];
		if (types[0] == 'i')
			d = (float) (* (int *) argv[0]);
		float s;
		if (types[0] == 'i')
			d = (float) (* (int *) argv[0]);
		if (argc > 1) {
			s = * (float *) argv[1];
			if (types[1] == 'i')
				s = (float) (* (int *) argv[1]);
		}
//		logMsg("Set %d SOS params to (%s)\n", argc, types);
		switch (selector) {					// switch on which parameter is being set
			case set_duration_f:
				mAEnv.setDuration(d);		break;
				mVEnv.setDuration(d);		break;
			case set_amplitude_f:
				mAEnv.setScale(d);		break;
			case set_frequency_f:
				mVEnv.setOffset(d);		break;			// freq is the offset of the vibrato envelope
				mFreq = d;				break;
			case set_position_f:
				mPanner.setPosition(d); 	break;
			case set_attack_f:
				mAEnv.setAttack(d);		break;
			case set_decay_f:
				mAEnv.setDecay(d);		break;
			case set_sustain_f:
				mAEnv.setSustain(d);		break;
			case set_release_f:
				mAEnv.setRelease(d);		break;
			default:
				logMsg(kLogError, "Unknown selector in SHARCAddInstrument set_parameter selector: %d\n", selector);
		 }
	} else  {
		logMsg(kLogError, "Unknown multi-arg (%d) setter in SHARCAddInstrument: %s\n", argc, types);
	}
}

// Play functions
// ffff = dur, amp, pitch, pos
// fffffffff = dur, amp, pitch, pos, att, dec, sus, rel

void SHARCAddInstrumentV::playOSC(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	unsigned nargs;

//	if (strcmp(types, "ffff") == 0)
	if (argc == 4)
		nargs = 4;
//	else if (strcmp(types, "fffffff") == 0)
	else if (argc == 8)
		nargs = 9;
	else {
		logMsg(kLogError, "Invalid type string in OSC message, expected 4 or 9 args got \"%s\"\n", types);
		return;
	}
	if (gVerbose)
		printf("\tSHARC1: d %5.2f   a %5.2f   f %7.1f   p %5.2f\n", *fargs[0], *fargs[1], *fargs[2], *fargs[3]);
	mAEnv.setDuration(*fargs[0]);			// dur
	mVEnv.setDuration(*fargs[0]);
	mAEnv.scaleValues(*fargs[1]);			// amp
	mFreq = *fargs[2];					// freq
	mVib.setFrequency(fRandM(5.0f, 8.0f));// vibrato freq
	mVib.setOffset(mFreq);
	mPanner.setPosition(*fargs[3]);		// pos
	if (nargs == 8) {
		printf("\t\ta %5.2f d %5.2f s %5.2f r %5.2f\n", fargs[4], fargs[5], fargs[6], fargs[7]);
		mAEnv.setAttack(*fargs[4]);
		mAEnv.setDecay(*fargs[5]);
		mAEnv.setSustain(*fargs[6]);
		mAEnv.setRelease(*fargs[7]);
	}
	if (mNoteFreq != mFreq) {
		getSHARCSpectrum(mInstr, mFreq, &mSOS);
		mNoteFreq = mFreq;
	}
	this->play();
}

void SHARCAddInstrumentV::playNote(float dur, float ampl, float pitch, float pos,
				float att, float dec, float sus, float rel, float chiff) {
	mAEnv.setDuration(dur);
	mAEnv.scaleValues(ampl);
	mVEnv.setOffset(pitch);
	mSOS.setFrequency(mVib);
	mFreq = pitch;
	mPanner.setPosition(pos);
	mAEnv.setAttack(att);
	mAEnv.setDecay(dec);
	mAEnv.setSustain(sus);
	mAEnv.setRelease(rel);
	if (mNoteFreq != mFreq) {
		getSHARCSpectrum(mInstr, mFreq, & mSOS);
		mNoteFreq = mFreq;
	}
	this->play();
}

void SHARCAddInstrumentV::playMIDI(float dur, int chan, int key, int vel) {
	mAEnv.setDuration(dur);
	mAEnv.scaleValues(sqrtf((float) vel / 128.0f));
	float frq = keyToFreq(key);
	mVEnv.setOffset(frq);
	mFreq = frq;
	if (mNoteFreq != mFreq) {
		getSHARCSpectrum(mInstr, mFreq, & mSOS);
		mNoteFreq = mFreq;
	}
	this->play();
}

#pragma mark SHARCAddInstrumentC // ------------------------------------------------------

// Why does this produce silence??
// ToDo: Fix it!

// Default constructor (unused)

SHARCAddInstrumentC::SHARCAddInstrumentC() :
		Instrument(),
		mAEnv(0.5, 0.05, 0.06, 0.5, 0.15),		// set up standard ADSR (1/2-sec duration)
		mMix(1),
//		mPanner(mMix, 0.0),
		mChiff(),
		mBPFilter(mChiff, BW_BAND_PASS, 1000.0f, 2.0f),
		mSOS(100.0f),							// SumOfSines init -- 1/f spectrum + noise
		mCEnv(2.0f, 0.05f, 0.05f, 0.0f, 0.2f) {	// chiff env - fast attack/decay
	this->init();
}

// c'tor with SHARCInstrument = the normal way to use this

SHARCAddInstrumentC::SHARCAddInstrumentC(SHARCInstrument * in) :
		Instrument(),
		mAEnv(1.0, 0.1, 0.1, 0.5, 0.4),			// ampl envelope
		mCEnv(1.0f, 0.02f, 0.05f, 0.0f, 0.2f),		// chiff env - fast attack/decay
		mSOS(100.0f),							// SumOfSines init -- 1/f spectrum + noise
		mMix(1),
		mPanner(),								// stereo panner
		mChiff(),
		mBPFilter(mChiff, BW_BAND_PASS, 1000.0f, 2.0f),
		mInstr(in) {
	this->init();								// call init fcn.
}

SHARCAddInstrumentC::~SHARCAddInstrumentC() { }
	
// init() = setup instance

void SHARCAddInstrumentC::init() {
	mNumChannels = 2;
	mSOS.setScale(mAEnv);
	mChiff.setScale(mCEnv);
	float chiffMix = fRandM(0.1f, 0.2f);
	mMix.addInput(mSOS);
	mMix.addInput(mBPFilter, chiffMix);
	mPanner.addInput(CSL_INPUT, mMix);

	mName = "SHARC";									// set graph's name
	mGraph = & mPanner;								// store the root of the graph as the inst var _graph
	mUGens["Osc"] = & mSOS;							// add ugens that can be monitored to the map
	mUGens["A env"] = & mAEnv;
	mUGens["Pos"] = & mPanner;

	mEnvelopes.push_back(& mAEnv);					// list envelopes for retrigger
	mEnvelopes.push_back(& mCEnv);
													// set up accessor vector
	mAccessors.push_back(new Accessor("du", set_duration_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("fr", set_frequency_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("po", set_position_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("at", set_attack_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("de", set_decay_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("su", set_sustain_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("re", set_release_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("ch", set_chiff_f, CSL_FLOAT_TYPE));
}
	
/// Plug functions

void SHARCAddInstrumentC::setParameter(unsigned selector, int argc, void **argv, const char *types) {
	if (argc == 1) {
		float d = * (float *) argv[0];
		if (types[0] == 'i')
			d = (float) (* (int *) argv[0]);
		float s;
		if (types[0] == 'i')
			d = (float) (* (int *) argv[0]);
		if (argc > 1) {
			s = * (float *) argv[1];
			if (types[1] == 'i')
				s = (float) (* (int *) argv[1]);
		}
//		logMsg("Set %d SOS params to (%s)\n", argc, types);
		switch (selector) {					// switch on which parameter is being set
			case set_duration_f:
				mAEnv.setDuration(d);		break;
				mCEnv.setDuration(d);		break;
			case set_amplitude_f:
				mAEnv.setScale(d);
				mCEnv.setScale(d);		break;
			case set_frequency_f:
				mSOS.setFrequency(d);				// freq is the offset of the vibrato envelope
				mFreq = d;				break;
			case set_position_f:
				mPanner.setPosition(d); 	break;
			case set_attack_f:
				mAEnv.setAttack(d);		break;
			case set_decay_f:
				mAEnv.setDecay(d);		break;
			case set_sustain_f:
				mAEnv.setSustain(d);		break;
			case set_release_f:
				mAEnv.setRelease(d);		break;
			default:
				logMsg(kLogError, "Unknown selector in SHARCAddInstrument set_parameter selector: %d\n", selector);
		 }
	} else  {
		logMsg(kLogError, "Unknown multi-arg (%d) setter in SHARCAddInstrument: %s\n", argc, types);
	}
}

// Play functions
// ffff = dur, amp, pitch, pos
// fffffffff = dur, amp, pitch, pos, att, dec, sus, rel

void SHARCAddInstrumentC::playOSC(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	unsigned nargs;

//	if (strcmp(types, "ffff") == 0)
	if (argc == 4)
		nargs = 4;
//	else if (strcmp(types, "fffffff") == 0)
	else if (argc == 8)
		nargs = 8;
	else {
		logMsg(kLogError, "Invalid type string in OSC message, expected 4 or 9 args got \"%s\"\n", types);
		return;
	}
	if (gVerbose)
		printf("\tSHARC2: d %5.2f   a %5.2f   f %7.1f   p %5.2f\n", *fargs[0], *fargs[1], *fargs[2], *fargs[3]);
	mAEnv.setDuration(*fargs[0]);			// dur
	mCEnv.setDuration(*fargs[0]);
	mAEnv.scaleValues(*fargs[1]);			// amp
	mCEnv.scaleValues(*fargs[1]);
	mFreq = *fargs[2];					// freq
	mSOS.setFrequency(mFreq);
	mPanner.setPosition(*fargs[3]);		// pos
	if (nargs == 8) {
		printf("\t\ta %5.2f d %5.2f s %5.2f r %5.2f\n", fargs[4], fargs[5], fargs[6], fargs[7]);
		mAEnv.setAttack(*fargs[4]);
		mAEnv.setDecay(*fargs[5]);
		mAEnv.setSustain(*fargs[6]);
		mAEnv.setRelease(*fargs[7]);
	}
	if (mNoteFreq != mFreq) {
		getSHARCSpectrum(mInstr, mFreq, & mSOS);
		mNoteFreq = mFreq;
	}
	this->play();
}

// missing MIDI fcns

#pragma mark VAdditiveInstrument // -------------------------------------------------------------------------------

// VAdditiveInstrument

// Constructor

VAdditiveInstrument::VAdditiveInstrument(SHARCSpectrum * spect1, SHARCSpectrum * spect2) :
		Instrument(),
		mAEnv(2.0f, 0.5f, 0.1f, 1.0f, 0.5f),	// set up standard ADSRs (2-sec duration)
//		mVEnv(kSquare, 2.0f, 0.5f, 1.0f, 0.1f, 1.0f, 0.2f),	// slower vibrato attack with initial delay
		mXEnv1(2.0f, 1.0f, 0.0001f, kSquare),	// set up x-fade lin-segs
		mXEnv2(2.0f, 0.0001f, 1.0f, kSquare),
		mSOS1(*spect1),						// SumOfSines init - use the given spectra
		mSOS2(*spect2),
//		mEMul1(mSOS1, mXEnv1),				// multiply cross-fade envs
//		mEMul2(mSOS2, mXEnv2),
//		mVibAdd(mVEnv, 110.0f),				// add vib env and freq
//		mSummer(mEMul1, mEMul2),
//		mAMul(mSummer, mAEnv),				// apply main ampl envelope
//		mPanner(mAMul, 0.0) {					// init the panner
		mSummer(mSOS1, mSOS2),
		mPanner(mSummer, 0.0) {					// init the panner
	this->init();
}

VAdditiveInstrument::VAdditiveInstrument(SHARCInstrument * i1, SHARCInstrument * i2) :
		Instrument(),
		mInstr1(i1),
		mInstr2(i2),
		mAEnv(2.0f, 0.5f, 0.1f, 1.0f, 0.5f),	// set up standard ADSRs (2-sec duration)
//		mVEnv(2.0f, 0.5f, 1.0f, 0.1f, 1.0f, 0.2f),		// slower vibrato attack with initial delay
		mXEnv1(1.0f, 1.0f, 0.0001f, kSquare),	// set up x-fade lin-segs
		mXEnv2(1.0f, 0.0001f, 1.0f, kSquare),
//		mEMul1(mSOS1, mXEnv1),				// multiply cross-fade envs
//		mEMul2(mSOS2, mXEnv2),
//		mVibAdd(mVEnv, 110.0f),				// add vib env and freq
//		mSummer(mEMul1, mEMul2),
//		mAMul(mSummer, mAEnv),				// apply main ampl envelope
//		mPanner(mAMul, 0.0) {					// init the panner
		mSummer(mSOS1, mSOS2),
		mPanner(mSummer, 0.0) {				// init the panner
	this->init();
}

// copy c'tor

//VAdditiveInstrument::VAdditiveInstrument(VAdditiveInstrument&) {
//
//}

VAdditiveInstrument::~VAdditiveInstrument() { }

void VAdditiveInstrument::init() {
	mNumChannels = 2;
	float vibDepth = fRandM(9.0f, 12.0f);
//	mVEnv.scaleValues(vibDepth);
//	mVib.setFrequency(fRandM(5.0f, 12.0f));	// vibrato freq
//	mVib.setScale(mVEnv);
//	mSOS1.setFrequency(mVib);					// set main freq to const + vibrato freq
//	mSOS2.setFrequency(mVib);
	mSOS1.setScale(mXEnv1);
	mSOS2.setScale(mXEnv2);
	mPanner.setScale(mAEnv);

	mName = "SpectralCrossFade";				// set graph's name
	mGraph = & mPanner;						// store the root of the graph as the inst var _graph
	mUGens["Osc1"] = & mSOS1;					// add ugens that can be monitored to the map
	mUGens["Osc2"] = & mSOS2;
	mUGens["A env1"] = & mAEnv;
	mUGens["X1 env"] = & mXEnv1;
	mUGens["X2 env"] = & mXEnv2;
	mUGens["Pos"] = & mSummer;
	
	mEnvelopes.push_back(& mAEnv);			// list envelopes for retrigger
//	mEnvelopes.push_back(& mVEnv);
	mEnvelopes.push_back(& mXEnv1);
	mEnvelopes.push_back(& mXEnv1);
											// set up accessor vector
	mAccessors.push_back(new Accessor("du", set_duration_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("fr", set_frequency_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("po", set_position_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("at", set_attack_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("de", set_decay_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("su", set_sustain_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("re", set_release_f, CSL_FLOAT_TYPE));
}

/// Plug functions

void VAdditiveInstrument::setParameter(unsigned selector, int argc, void **argv, const char *types) {
	if (argc == 1) {
		float d = * (float *) argv[0];
		if (types[0] == 'i')
			d = (float) (* (int *) argv[0]);
		float s;
		if (types[0] == 'i')
			d = (float) (* (int *) argv[0]);
		if (argc > 1) {
			s = * (float *) argv[1];
			if (types[1] == 'i')
				s = (float) (* (int *) argv[1]);
		}
		logMsg("Set %d V_SOS params to (%s)\n", argc, types);
		switch (selector) {					// switch on which parameter is being set
			case set_duration_f:
				mAEnv.setDuration(d);
//				mVEnv.setDuration(d);
				mXEnv1.setDuration(d);
				mXEnv2.setDuration(d);
				break;
			case set_amplitude_f:
				mAEnv.setScale(d);
				break;
			case set_frequency_f:
				mFreq = d;
				mSOS1.setFrequency(d);
				mSOS2.setFrequency(d);
//				mVibAdd.setOperand(d);
				if (mInstr1)
					this->getSpectra();
				break;
			case set_position_f:
				mPanner.setPosition(d); 	break;
			case set_attack_f:
				mAEnv.setAttack(d);
				break;
			case set_decay_f:
				mAEnv.setDecay(d);
				break;
			case set_sustain_f:
				mAEnv.setSustain(d);
				break;
			case set_release_f:
				mAEnv.setRelease(d);
				break;
			default:
				logMsg(kLogError, "Unknown selector in VAdditiveInstrument set_parameter selector: %d\n", selector);
		}
	} else {			// multiple args
		logMsg(kLogError, "Unknown multi-arg (%d) setter in AdditiveInstrument: %s\n", argc, types);
	}
}

/// Play functions
/// OSC note formats (4 or 8 arguments):
/// 	dur, ampl, freq, pos
/// 	dur, ampl, freq, pos, att, dec, sus, rel

void VAdditiveInstrument::playOSC(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	unsigned nargs;
	
	if (argc == 4)
		nargs = 4;
	else if (argc == 8)
		nargs = 8;
	else {
		logMsg(kLogError, "Invalid type string in OSC message, expected \"ff...ff\" got \"%s\"\n", types);
		return;
	}
	if (gVerbose)
		printf("\tV_Add: d %5.2f   a %5.2f   f %7.1f   p %5.2f\n", *fargs[0], *fargs[1], *fargs[2], *fargs[3]);
	float dur = *fargs[0];
	mAEnv.setDuration(dur);
	mXEnv1.setDuration(dur);
	mXEnv2.setDuration(dur);
	mAEnv.scaleValues(*fargs[1]);
	mFreq = *fargs[2];
	mSOS1.setFrequency(mFreq);
	mSOS2.setFrequency(mFreq);
	mPanner.setPosition(*fargs[3]);
	if (nargs == 8) {
//		printf("\t\ta %5.2f d %5.2f s %5.2f r %5.2f\n", fargs[4], fargs[5], fargs[6], fargs[7]);
		mAEnv.setAttack(*fargs[4]);
		mAEnv.setDecay(*fargs[5]);
		mAEnv.setSustain(*fargs[6]);
		mAEnv.setRelease(*fargs[7]);
	}
	if ((mInstr1) && (mNoteFreq != mFreq)) {
		getSHARCSpectrum(mInstr1, mFreq, & mSOS1);
		getSHARCSpectrum(mInstr2, mFreq, & mSOS2);
		mNoteFreq = mFreq;
	}
	this->play();
}

void VAdditiveInstrument::playNote(float dur, float ampl, float freq, float pos, float att, float dec, float sus, float rel) {
	mAEnv.setDuration(dur);
	mXEnv1.setDuration(dur);
	mXEnv2.setDuration(dur);
	mAEnv.scaleValues(ampl);
	mFreq = freq;
	mPanner.setPosition(pos);
	mAEnv.setAttack(att);
	mAEnv.setDecay(dec);
	mAEnv.setSustain(sus);
	mAEnv.setRelease(rel);
	if (mInstr1)
		this->getSpectra();
	this->play();
}

void VAdditiveInstrument::playMIDI(float dur, int chan, int key, int vel) {
	mAEnv.setDuration(dur);
	mXEnv1.setDuration(dur);
	mXEnv2.setDuration(dur);
	mAEnv.scaleValues(sqrtf((float) vel / 128.0f));
	mSOS1.setFrequency(keyToFreq(key));
	mSOS2.setFrequency(keyToFreq(key));
	if (mInstr1)
		this->getSpectra();
	this->play();
}

// recompute the SOS spectrum from the SHARC data

void VAdditiveInstrument::getSpectra() {
	if ( ! mInstr1) return;
	if ( ! mInstr2) return;
	if (mNoteFreq == mFreq) return;
	
	mSOS1.clearPartials();
	SHARCSpectrum * spect = mInstr1->spectrum_with_frequency(mFreq);
	for (unsigned i = 0; i < spect->_num_partials; i++) {
		Partial * harm = spect->_partials[i];
		mSOS1.addPartial(harm->number, harm->amplitude, harm->phase);
	}
	mSOS1.createCache();							// make the cached wavetable
	
	mSOS2.clearPartials();
	spect = mInstr2->spectrum_with_frequency(mFreq);
	for (unsigned i = 0; i < spect->_num_partials; i++) {
		Partial * harm = spect->_partials[i];
		mSOS2.addPartial(harm->number, harm->amplitude, harm->phase);
	}
	mSOS2.createCache();							// make the cached wavetable
	mNoteFreq = mFreq;							// remember last freq
}

#pragma mark VAdditiveInstrumentR // ------------------------------------------------------

// VAdditiveInstrumentR

// Constructor

# if 0 // still not debugged

VAdditiveInstrumentR::VAdditiveInstrumentR(SHARCSpectrum * spect1, SHARCSpectrum * spect2) :
		VAdditiveInstrument(spect1, spect2),
		mREnv(2, 1.0, 0.5, 0.2),				// ampl env = random (frq, amp, off, step)
		mESub(mREnv, 1.0) {
	this->init();
}

VAdditiveInstrumentR::VAdditiveInstrumentR(SHARCInstrument * i1, SHARCInstrument * i2) :
		VAdditiveInstrument(i1, i2),
		mREnv(2, 1.0, 0.5, 0.2),				// ampl env = random (frq, amp, off, step)
		mESub(mREnv, 1.0) {
	this->init();
}

VAdditiveInstrumentR::~VAdditiveInstrumentR() { }

void VAdditiveInstrumentR::init() {
	mNumChannels = 2;
	mName = "SpectralCrossFade";				// set graph's name
	mGraph = & mPanner;						// store the root of the graph as the inst var _graph
	mEMul1.addInput(CSL_INPUT, mSOS1);			// low-level UGen patching here
	mEMul1.addInput(CSL_OPERAND, mREnv);
	mEMul2.addInput(CSL_INPUT, mSOS2);
	mEMul2.addInput(CSL_OPERAND, mESub);
	float vibDepth = fRandM(9.0f, 15.0f);
	mVEnv.scaleValues(vibDepth);
	mVib.setFrequency(fRandM(5.0f, 12.0f));	// vibrato freq
	mVib.setScale(mVEnv);
	mSOS1.setFrequency(mVibAdd);				// set main freq to const + vibrato freq
	mSOS2.setFrequency(mVibAdd);

	mUGens["Osc1"] = & mSOS1;					// add ugens that can be monitored to the map
	mUGens["Osc2"] = & mSOS2;
	mUGens["A env1"] = & mAEnv;
	mUGens["X1 env"] = & mXEnv1;
	mUGens["X2 env"] = & mXEnv2;
	mUGens["Pos"] = & mSummer;
	
	mEnvelopes.push_back(& mAEnv);				// list envelopes for retrigger
	mEnvelopes.push_back(& mXEnv1);
	mEnvelopes.push_back(& mXEnv1);
	mEnvelopes.push_back(& mVEnv);
												// set up accessor vector
	mAccessors.push_back(new Accessor("du", set_duration_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("fr", set_frequency_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("po", set_position_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("at", set_attack_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("de", set_decay_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("su", set_sustain_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("re", set_release_f, CSL_FLOAT_TYPE));
}

/// Plug functions

void VAdditiveInstrumentR::setParameter(unsigned selector, int argc, void **argv, const char *types) {
	if (argc == 1) {
		float d = * (float *) argv[0];
		if (types[0] == 'i')
			d = (float) (* (int *) argv[0]);
		float s;
		if (types[0] == 'i')
			d = (float) (* (int *) argv[0]);
		if (argc > 1) {
			s = * (float *) argv[1];
			if (types[1] == 'i')
				s = (float) (* (int *) argv[1]);
		}
		logMsg("Set %d V_SOS params to (%s)\n", argc, types);
		switch (selector) {					// switch on which parameter is being set
			case set_duration_f:
				mAEnv.setDuration(d);
				mVEnv.setDuration(d);
				mXEnv1.setDuration(d);
				mXEnv2.setDuration(d);
				break;
			case set_amplitude_f:
				mAEnv.setScale(d);
				break;
			case set_frequency_f:
				mFreq = d;
				mVibAdd.setOperand(mFreq);
				if (mInstr1)
					this->getSpectra();
				break;
			case set_position_f:
				mPanner.setPosition(d); 	break;
			case set_attack_f:
				mAEnv.setAttack(d);
				break;
			case set_decay_f:
				mAEnv.setDecay(d);
				break;
			case set_sustain_f:
				mAEnv.setSustain(d);
				break;
			case set_release_f:
				mAEnv.setRelease(d);
				break;
			default:
				logMsg(kLogError, "Unknown selector in VAdditiveInstrument set_parameter selector: %d\n", selector);
		}
	} else {			// multiple args
		logMsg(kLogError, "Unknown multi-arg (%d) setter in AdditiveInstrument: %s\n", argc, types);
	}
}

/// Play functions
/// OSC note formats (4 or 8 arguments):
/// 	dur, ampl, freq, pos
/// 	dur, ampl, freq, pos, att, dec, sus, rel

void VAdditiveInstrumentR::playOSC(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	unsigned nargs;
	
	if ((argc == 4) && (strcmp(types, "ffff") == 0))
		nargs = 4;
	else if ((argc == 8) && (strcmp(types, "ffffffff") == 0))
		nargs = 8;
	else {
		logMsg(kLogError, "Invalid type string in OSC message, expected \"ff...ff\" got \"%s\"\n", types);
		return;
	}
//	if (gVerbose)
		printf("\tV_AddR: d %5.2f   a %5.2f   f %7.1f   p %5.2f\n", *fargs[0], *fargs[1], *fargs[2], *fargs[3]);
	float dur = *fargs[0];
	mAEnv.setDuration(dur);
	mXEnv1.setDuration(dur);
	mXEnv2.setDuration(dur);
	mVEnv.setDuration(dur);
	mAEnv.scaleValues(dur);
	mFreq = *fargs[2];
	mVibAdd.setOperand(mFreq);
	mPanner.setPosition(*fargs[3]);
	if (nargs == 8) {
//		printf("\t\ta %5.2f d %5.2f s %5.2f r %5.2f\n", fargs[4], fargs[5], fargs[6], fargs[7]);
		mAEnv.setAttack(*fargs[4]);
		mAEnv.setDecay(*fargs[5]);
		mAEnv.setSustain(*fargs[6]);
		mAEnv.setRelease(*fargs[7]);
	}
	if (mInstr1)
		this->getSpectra();
	this->play();
}
#endif
