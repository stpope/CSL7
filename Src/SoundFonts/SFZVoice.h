#ifndef SFZVoice_h
#define SFZVoice_h

#include "../JuceLibraryCode/JuceHeader.h"
#include "SFZEG.h"
#include "CSL_Core.h"				// STP adds - include CSL core

namespace SFZero {

class SFZRegion;

class JUCE_API SFZVoice : public juce::SynthesiserVoice {
	public:
		SFZVoice();
		~SFZVoice();

		bool	canPlaySound(juce::SynthesiserSound* sound);
		void	startNote(
				const int midiNoteNumber,
				const float velocity,
				juce::SynthesiserSound* sound,
				const int currentPitchWheelPosition);
		void	stopNote(float velocity, const bool allowTailOff);
		void	stopNoteForGroup();
		void	stopNoteQuick();
		void	pitchWheelMoved(const int newValue);
		void	controllerMoved(
				const int controllerNumber,
				const int newValue);
		void	renderNextBlock(
				juce::AudioSampleBuffer& outputBuffer,
				int startSample, int numSamples);
		bool	isPlayingNoteDown();
		bool	isPlayingOneShot();

		int	getGroup();
		int	getOffBy();

		// Set the region to be used by the next startNote().
		void	setRegion(SFZRegion* nextRegion);

		juce::String	infoString();

					/// Fill a buffer of Frames -- this is the core CSL "pull" function;
					/// the given buffer can be written into, and a changed() message is sent.
//	void nextBuffer(csl::Buffer & outputBuffer) noexcept(false);

	protected:
		int       	trigger;
		SFZRegion*	region;
		int       	curMidiNote, curPitchWheel;
		double    	pitchRatio;
		float     	noteGainLeft, noteGainRight;
		double    	sourceSamplePosition;
		SFZEG     	ampeg;
		unsigned long	sampleEnd;
		unsigned long	loopStart, loopEnd;

		// Info only.
		unsigned long	numLoops;
		int	curVelocity;

		void	calcPitchRatio();
		void	killNote();
		double	noteHz(double note, const double freqOfA = 440.0);
	};

}

#endif 	// SFZVoice_h
