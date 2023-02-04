//
///  SoundFontInstrument.cpp -- SoundFont instrument
///	This uses Steve Folta's SFZero open-source code from https://github.com/stevefolta/SFZero
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT

#include "SoundFontInstrument.h"
#include <math.h>

using namespace csl;
using namespace SFZero;

// stopMe = forked fcn to stop playng

static void stopMe(void * theArg) {
	SoundFontInstrument * instr = (SoundFontInstrument * ) theArg;
	int chan = instr->mLastChannel;
	int key = instr->mLastNoteNumber;
	float vel = instr->mLastVelocity;
	sleepSec(instr->mDur);
	instr->noteOff(chan, key, vel, true);
}

// SoundFontInstrument c'tor

SoundFontInstrument::SoundFontInstrument(String fName) :
			Instrument(),
			SFZSynth(),
			mSFFile(0),
			mNumVoices(16) {
	this->init();
	if (fName.length() > 0)
		this->load(fName);
	this->setCurrentPlaybackSampleRate(CGestalt::frameRateF());
}

// SoundFontInstrument d'tor

SoundFontInstrument::~SoundFontInstrument() {
	if (stopperThread != 0) {
		stopperThread->stopThread(0);
		delete stopperThread;
	}
}

// setup instance

void SoundFontInstrument::init() {
	mNumChannels = 2;
	mName = "SoundFont Player";					// set graph's name
	mGraph = this;								// store the root of the graph as the inst var _graph
	mEnvelopes.push_back(this);					// list envelopes for retrigger
												// set up accessor vector
	mAccessors.push_back(new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("fr", set_frequency_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("po", set_position_f, CSL_FLOAT_TYPE));
}

// Load/parse SoundFont file

void SoundFontInstrument::load(String fName) {
	if (mSFFile)
		delete mSFFile;
	mSFFile = new File(fName);
	if (!mSFFile->existsAsFile()) {
		logMsg(kLogError, "Missing SoundFont file: %s\n", mSFFile->getFullPathName().toUTF8());
		return;
	}
	logMsg("Opening SoundFont file: %s\n", mSFFile->getFullPathName().toUTF8());
	String extension = mSFFile->getFileExtension();
	SFZero::SFZSound * tSnd;
	if (extension == ".sf2" || extension == ".SF2") {
		tSnd = new SFZero::SF2Sound(*mSFFile);
		mMode = kSF2;
	} else {
		tSnd = new SFZero::SFZSound(*mSFFile);
		mMode = kSFZ;
	}
	clearVoices();						// clear, then add mNumVoices voices
	for (int i = 0; i < mNumVoices; i++) {
		SFZVoice * newVoice = new SFZVoice;
		newVoice->setCurrentPlaybackSampleRate (CGestalt::frameRateF());
		this->addVoice(newVoice);
	}
	clearSounds();
	tSnd->loadRegions();
	juce::AudioFormatManager fManager;
	tSnd->loadSamples(&fManager);
	this->addSound(tSnd);
}

// Describe the loaded sound font

//		SoundFontInstrument with 2 regions and 9 subsounds
//		Region: keys 36 to 59, vels 0 to 127
//		Region: keys 60 to 89, vels 0 to 127
//		Subsound named:0: Orgue1
//		Subsound named:1: orgue1 leslie
//		Subsound named:2: orgue1 verby
//		Subsound named:3: orgue2
//		Subsound named:4: orgue2 leslie
//		Subsound named:5: orgue2 verby
//		Subsound named:6: orgue3
//		Subsound named:7: orgue3 leslie
//		Subsound named:8: orgue3 verby
//		SelectedSubsound = 0
//		Preset 0:0 = Orgue1
//		Preset 0:1 = orgue1 leslie
//		Preset 0:2 = orgue1 verby
//		Preset 0:3 = orgue2
//		Preset 0:4 = orgue2 leslie
//		Preset 0:5 = orgue2 verby
//		Preset 0:6 = orgue3
//		Preset 0:7 = orgue3 leslie
//		Preset 0:8 = orgue3 verby

void SoundFontInstrument::dump() {
	SFZero::SFZSound * tSnd = (SFZero::SFZSound *)getSound(0).get();
	int nRegs = tSnd->getNumRegions();
	int nSnds = tSnd->numSubsounds();

	printf("SoundFontInstrument with %d regions and %d subsounds\n", nRegs, nSnds);
	for (int i = 0; i < nRegs; i++) {
		SFZRegion* reg = tSnd->regionAt(i);
		printf("Region: keys %u to %u, vels %u to %u\n", reg->lokey, reg->hikey, reg->lovel, reg->hivel);
	}
	for (int i = 0; i < nSnds; i++) {
		printf("Subsound named:%s\n", tSnd->subsoundName(i).toUTF8());
	}
	printf("SelectedSubsound = %d\n", tSnd->selectedSubsound());
	
	for (SFZSampleMap::Iterator pos(tSnd->samples); pos.next();) {
		printf("Sample: %s\n", pos.getKey().toUTF8());
	}
	if (mMode == kSF2) {
		SF2Sound * mSF2 = (SF2Sound *)tSnd;
		for (int i = 0; i < mSF2->presets.size(); i++) {
			SF2Sound::Preset * ps = mSF2->presets[i];
			printf("Preset %d:%d = %s\n", ps->bank, ps->preset, ps->name.toUTF8());
		}
	}
	printf("\n");
}

String SoundFontInstrument::getSubSndName (int which) {
	SFZero::SFZSound * tSnd = (SFZero::SFZSound *)getSound(0).get();
	return tSnd->subsoundName(which);
}

// trigger a new note with params

void SoundFontInstrument::trigger(float dur, int chan, int key, int vel, float pos) {
	float mappedVel = VelToRatio(vel);
	this->noteOn(chan, key, mappedVel, pos);					// play!
	mDur = dur;												// store params for stopping
	mLastChannel = chan;
	mLastNoteNumber = key;
	mLastVelocity = mappedVel;
	if (stopperThread == NULL)
		stopperThread = CThread::MakeThread();
	stopperThread->createThread(stopMe, this);					// fork a thread to stop playing
}

// Plug functions

void SoundFontInstrument::setParameter(unsigned selector, int argc, void **argv, const char *types) {
	if ((selector == set_file_f) && (argc == 1)) {			// set file
		char * fn = (char *) argv[0];
		this->load(String(fn));
	}
}

// Play functions
// Play a note with a given arg list
// Formats:
// 	dur, chan, key, ampl, pos

void SoundFontInstrument::playOSC(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	int ** iargs = (int **) argv;
							// pitch, ampl, dur, pos
	if (strcmp(types, "fffff") != 0) {
		logMsg(kLogError, "Invalid type string in OSC message, expected \"ffff\" got \"%s\"", types);
		return;
	}
	float dur = *fargs[0];
	int instr = *iargs[1];				// instr is MIDFI channel
	int pitch = *iargs[2];				// pitch in MIDI keys
	float ampl = *fargs[3] * 127.0;		// scale amp to 0-127
	float pos = *fargs[4] * 100.0f;		// scale pos to +-100
	
	this->trigger(dur, instr, pitch, ampl, pos);		// start playing
}

void SoundFontInstrument::playNote(float ampl, float c_fr, float pos) {
//	this->trigger(dur, chan, key, vel);				// start playing
}

void SoundFontInstrument::playMIDI(float dur, int chan, int key, int vel) {
	this->trigger(dur, chan, key, vel);				// start playing
}

// nextBuffer = Sample creation, create an AudioSampleBuffer with the output ptrs and call renderNextBlock()

void SoundFontInstrument::nextBuffer(Buffer & outputBuffer) noexcept(false) {
	AudioSampleBuffer asBuf(outputBuffer.buffers(), outputBuffer.mNumChannels, outputBuffer.mNumFrames);
	MidiBuffer midiMessages;
	this->renderNextBlock(asBuf, midiMessages, 0, outputBuffer.mNumFrames);
}

void SoundFontInstrument::noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff) {
	SFZSynth::noteOff(midiChannel, midiNoteNumber, velocity, allowTailOff);
}
