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
#include "SFZSound.h"
#include "SF2Sound.h"
//#include "SFZSynth.h"
#include "SFZVoice.h"
#include "SFZDebug.h"

#define VELSCALE 64.0f /* Mike Mcnabb says this is right. */
#define VelToRatio(vel0)	((powf(10.0f, (vel0 - VELSCALE) / VELSCALE)) / 10.0f)

namespace csl {

///
/// CSL SoundFont loader/player; based on Steve Folta's open-source code from https://github.com/stevefolta/SFZero
///

class SoundFontInstrument : public Instrument {
public:
	SoundFontInstrument(String fName);

	void trigger();
	void trigger(float dur, int chan, int key, int vel);
					// Plug function
	void setParameter(unsigned selector, int argc, void **argv, const char *types);
					// Play functions
	void playOSC(int argc, void **argv, const char *types);
	void playNote(float ampl = 0.25f, float c_fr = 220.0f, float pos = 0.0f);
	void playMIDI(float dur, int chan, int key, int vel);
	
	void nextBuffer(Buffer & outputBuffer) noexcept(false) override;	///< Sample creation

	SFZero::SFZVoice mSFVox;
	SFZero::SFZSound * mSFSound;
	float mDur, mVel;

protected:
	void init();
	void load();
//	void selectSample();
//	void selectLayer();
	
	File mSFFile;
	CThread * stopperThread = 0;			///< thread I fork to stop me
};

} // CSL

#endif
