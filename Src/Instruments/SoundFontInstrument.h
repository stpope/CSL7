//
//  SoundFontInstrument.h -- SoundFont loader/oscillator instrument
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	This uses Steve Folta's SFZero open-source code from https://github.com/stevefolta/SFZero

#ifndef INCLUDE_SoundFont_H
#define INCLUDE_SoundFont_H

#include "Instrument.h"
#include "SoundFile.h"
#include "Oscillator.h"

#include "SFZSound.h"			// Steve F's includes
#include "SF2Sound.h"
#include "SFZSynth.h"
#include "SFZVoice.h"
#include "SFZDebug.h"

#define VELSCALE 64.0f		 /* Mike Mcnabb says this is right. */
#define VelToRatio(vel0)	((powf(10.0f, (vel0 - VELSCALE) / VELSCALE)) / 7.0f)

namespace csl {

/// Which format file?

typedef enum {
	kSFZ,
	kSF2
} sfMode;

///
/// CSL SoundFont loader/player; based on Steve Folta's open-source code from https://github.com/stevefolta/SFZero
///

class SoundFontInstrument : public Instrument, SFZero::SFZSynth {
public:
	SoundFontInstrument(String fName);
	~SoundFontInstrument();

	void trigger(float dur = 1.0f, int chan = 1, int key = 60, int vel = 120);
	void dump();							///< Describe the loaded sound font
					/// Plug function
	void setParameter(unsigned selector, int argc, void **argv, const char *types);
					/// Play functions
	void playOSC(int argc, void **argv, const char *types);
	void playNote(float ampl = 0.25f, float c_fr = 220.0f, float pos = 0.0f);
	void playMIDI(float dur, int chan, int key, int vel);
	
	void nextBuffer(Buffer & outputBuffer) noexcept(false) override;	///< Sample creation
	void noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff);

//	OwnedArray<SFZero::SFZVoice> mSFVoices;
//	SFZero::SFZSound * mSFSound = 0;
	float mDur, mVel;
	int mLastChannel, mLastNoteNumber;
	float mLastVelocity;

protected:
	void init();
	void load();
//	SFZero::SFZVoice * find_voice();

	File mSFFile;
	CThread * stopperThread = 0;			///< thread I fork to stop notes
	sfMode mMode;
	int mNumVoices;
};

} // CSL

#endif
