//
//  JUCEIO.h -- Driver IO object for CSL on JUCE
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef CSL_JUCEIO_H
#define CSL_JUCEIO_H

#include "CSL_Core.h"
//#include "juce_AudioIODevice.h"
#include "../../JuceLibraryCode/JuceHeader.h"

namespace csl {

///
/// JUCEIO is an IO that runs as a JUCE client
///

class JUCEIO : public IO, public juce::AudioIODeviceCallback {
public:				
											///< Constructor (stereo by default)
	JUCEIO(unsigned s_rate = CSL_mFrameRate, unsigned b_size = CSL_mBlockSize, 
			int in_device = 0, int out_device = 0, 
			unsigned in_chans = 0, unsigned out_chans = 2);
	virtual ~JUCEIO();

	void open() noexcept(false);			///< open/close start/stop methods
	void close() noexcept(false);
	void start() noexcept(false);			///< start my timer thread
	void stop() noexcept(false);			///< stop the timer thread

											///< Audio playback callback & utilities
	void audioDeviceIOCallbackWithContext(const float *const * inputChannelData, int numInputChannels,
									float * const * outputChannelData, int numOutputChannels,
									int numSamples, const juce::AudioIODeviceCallbackContext & context) override;
											/// JUCE methods
	void audioDeviceAboutToStart (juce::AudioIODevice*);
	void audioDeviceStopped();

protected:
    juce::AudioDeviceManager mAudioDeviceManager;		///< JUCE AudioDeviceManager
    juce::AudioIODevice * mDevice;                  ///< JUCE audio IO device ptr
};

}	// end of namespace

#endif // CSL_JACKIO_H
