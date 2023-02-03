#ifndef SFZSound_h
#define SFZSound_h

#include "../JuceLibraryCode/JuceHeader.h"
#include "SFZRegion.h"

namespace SFZero {

class SFZSample;

typedef juce::HashMap<juce::String, SFZSample*> SFZSampleMap;

class SFZSound : public juce::SynthesiserSound {
	public:
		SFZSound(const juce::File& file);
		~SFZSound();

		bool	appliesToNote(const int midiNoteNumber);
		bool	appliesToChannel(const int midiChannel);

		void	addRegion(SFZRegion* region); 	// Takes ownership of the region.
		SFZSample*	addSample(juce::String path, juce::String defaultPath = juce::String());
		void	addError(const juce::String& message);
		void	addUnsupportedOpcode(const juce::String& opcode);

		virtual void	loadRegions();
		virtual void	loadSamples(
			juce::AudioFormatManager* formatManager,
			double* progressVar = NULL, juce::Thread* thread = NULL);

		SFZRegion*	getRegionFor(
			int note, int velocity, SFZRegion::Trigger trigger = SFZRegion::attack);
		int	getNumRegions();
		SFZRegion*	regionAt(int index);

		juce::String	getErrorsString();

		virtual int	numSubsounds();
		virtual juce::String	subsoundName(int whichSubsound);
		virtual void	useSubsound(int whichSubsound);
		virtual int 	selectedSubsound();

		void	dump();
	
		SFZSampleMap	samples;			// STP change - make this public

	protected:
		juce::File 	file;
		juce::Array<SFZRegion*>	regions;
		juce::StringArray      	errors;
		juce::HashMap<juce::String, juce::String>	unsupportedOpcodes;
	};

}


#endif 	// SFZSound_h

