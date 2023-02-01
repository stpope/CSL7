//
//  SoundFontInstrument.cpp -- SoundFont instrument
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	This uses Steve Folta's SFZero open-source code from https://github.com/stevefolta/SFZero

#include "SoundFontInstrument.h"
#include <math.h>

using namespace csl;

// forked fcn to stop playng

static void stopMe(void * theArg) {
	SoundFontInstrument * instr = (SoundFontInstrument * ) theArg;
	sleepSec(instr->mDur);
	instr->mSFVox.stopNote(instr->mVel, true);
}

// SoundFontInstrument c'tor

SoundFontInstrument::SoundFontInstrument(String fName) :
			Instrument(),
			mSFFile(fName) {
	this->init();
	this->load();
}

// setup instance

void SoundFontInstrument::init() {
	mNumChannels = 2;
	mName = "SoundFont Player";					// set graph's name
	mGraph = this;								// store the root of the graph as the inst var _graph
	//	mUGens["String"] = & mString;				// add ugens that can be monitored to the map
	mEnvelopes.push_back(this);					// list envelopes for retrigger
												// set up accessor vector
	mAccessors.push_back(new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("fr", set_frequency_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("po", set_position_f, CSL_FLOAT_TYPE));
}

// Load/parse SoundFont file

void SoundFontInstrument::load() {
	if (!mSFFile.existsAsFile()) {
		logMsg(kLogError, "Missing SoundFont file: %s\n", mSFFile.getFullPathName().toUTF8());
		return;
	}
	logMsg("Opening SoundFont file: %s\n", mSFFile.getFullPathName().toUTF8());
	String extension = mSFFile.getFileExtension();
	if (extension == ".sf2" || extension == ".SF2")
		mSFSound = new SFZero::SF2Sound(mSFFile);
	else
		mSFSound = new SFZero::SFZSound(mSFFile);
	mSFSound->loadRegions();
	juce::AudioFormatManager fManager;
	mSFSound->loadSamples(&fManager);
}

void SoundFontInstrument::trigger() {
	int midiNoteNumber = 60;
	mDur = 1.0;
	mVel = 0.5;;
	mSFVox.startNote(midiNoteNumber, mVel, mSFSound, 8192);
}

void SoundFontInstrument::trigger(float dur, int chan, int key, int vel) {
	float fltVel = VelToRatio(vel);
	mSFVox.startNote(key, fltVel, mSFSound, 8192);
	if (stopperThread == NULL)
		stopperThread = CThread::MakeThread();			// fork a thread to stop playing
	stopperThread->createThread(stopMe, this);
}

// Plug functions

void SoundFontInstrument::setParameter(unsigned selector, int argc, void **argv, const char *types) {

}

// Play functions
// Play a note with a given arg list
// Formats:
// 	pitch, ampl, dur, pos

void SoundFontInstrument::playOSC(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	int ** iargs = (int **) argv;
	float pitch, ampl, dur, pos;
							// pitch, ampl, dur, pos
	if (strcmp(types, "ffff") != 0) {
		logMsg(kLogError, "Invalid type string in OSC message, expected \"ffff\" got \"%s\"", types);
		return;
	}
	pitch = *fargs[0];
	ampl = *fargs[1];
	dur = *fargs[2];
	pos = *fargs[3];
	
	this->trigger(dur, 0, (int)pitch, ampl);							// start playing

}

void SoundFontInstrument::playNote(float ampl, float c_fr, float pos) {
	
}

void SoundFontInstrument::playMIDI(float dur, int chan, int key, int vel) {
	this->trigger(dur, chan, key, vel);								// start playing

}

// nextBuffer = Sample creation, delegate to the SFVoice

void SoundFontInstrument::nextBuffer(Buffer & outputBuffer) noexcept(false) {
	mSFVox.nextBuffer(outputBuffer);
}

