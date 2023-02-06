//
///  SoundFontInstrument.cpp -- SoundFont instrument
///
///	This uses Steve Folta's SFZero open-source code from https://github.com/stevefolta/SFZero
///
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT

#ifndef INCLUDE_SoundFont_H
#define INCLUDE_SoundFont_H

#include "Instrument.h"
#include "SoundFile.h"
#include "Oscillator.h"

#include "SFZSound.h"			// SFZero includes
#include "SF2Sound.h"
#include "SFZSynth.h"
#include "SFZVoice.h"
#include "SFZDebug.h"

#define VELSCALE 64.0f		 /* Mike Mcnabb says this is right. */
#define VelToRatio(vel0)	(csl_min(((powf(10.0f, (vel0 - VELSCALE) / VELSCALE)) / 7.0f), 1.0f))

namespace csl {

/// Which format SoundFont file?

typedef enum {
	kSFZ,
	kSF2
} sfMode;


/// Data structure for the playing notes; the nextBuffer() method checks to schedule note-off commands

class ThreadData {
public:
	ThreadData() {		///< c'tor zeroes it out
		mStop = (int64)0;
		mChan = 1;
		mNote = 1;
		mVel = 0.0f;
	};
	
	int64 mStop;			///< stop (clock) time in msec
	float mVel;			///< note params
	int mChan, mNote;
};

///
/// CSL SoundFont loader/player; based on Steve Folta's open-source code from https://github.com/stevefolta/SFZero
///

class SoundFontInstrument : public Instrument, SFZero::SFZSynth {
public:
	SoundFontInstrument(String fName = "");
	~SoundFontInstrument();

	void load(String fName);				///< load a SoundFont into an instrument
										/// play a note
	void trigger(float dur = 1.0f, int chan = 1, int key = 60, int vel = 120, float pos = 0.0f);
	void dump();							///< Describe the loaded sound font
	String getSubSndName (int which);
	bool isActive();
					/// Plug function - used to load files
	void setParameter(unsigned selector, int argc, void **argv, const char *types);
					/// Play functions
	void playOSC(int argc, void **argv, const char *types);
	void playNote(float ampl = 0.25f, float c_fr = 220.0f, float pos = 0.0f);
	void noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff);
	void playMIDI(float dur, int chan, int key, int vel);
	
	void nextBuffer(Buffer & outputBuffer) noexcept(false) override;	///< Sample creation

	int64 mNextToStop;	  					///< end time of the next note to stop
	int mNextToStopInd;	  					///< index of data pool corresponding to next note to stop
	std::vector<ThreadData *> mPlayingNotes;	///< list of playing notes

protected:
	void init();							///< init fcn
	int64 setNextStopTime();				///< find the next note stop time
	void stopNote(int index);
	
	File * mSFFile;						///< my SoundFont file
	sfMode mMode;						///< SFZ or SF2 file type
	int mNumVoices;
};

} // CSL

#endif
