//
//  OSC_main.cpp -- CSL "main" function for use with OSC input triggering CSL instrument objects.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This version takes an instrument library (Instrument ** library) and creates an OSC address space for 
// its instruments.  In this test, we create a library with 4 FM instruments and 4 sound file players.
//
// The address space is structured as follows:
//		For each instrument in the library (which is created in the main() function), we create  
//			a top-level OSC node called "in" where n is a number.
//		Each instrument node has children for all the instrument's accessors, and "p" to play 
//			a note and "pn" to play  a note passing parameters to the instrument
// There's a top-level "q" command to quit CSL.

/********** Example OSC address space:
*
	/			-- root
		/q:		quit command
	/i1/			instrument 1 (basic FM instrument)
		/i1/du:	set-duration command
		/i1/am:	set-amplitude command
		/i1/in:		...other setters
		/i1/cf:
		/i1/mf:
		/i1/aa:		-- these are the defined accesors in the instrument; see FM_Instrument.cpp
		/i1/ad:
		/i1/as:
		/i1/ar:
		/i1/ia:
		/i1/id:
		/i1/is:
		/i1/ir:
		/i1/p:		simple play-note command
		/i1/pn:	play-note-with-parameters command
	[ ... ]			...other instruments
	/i8/			i8 is a sample-player instrument
		/i8/am:	set-amplitude command
		/i8/ra:	set playback rate
		/i8/po:	set position
		/i8/fi: set-file-name   -- accessors defined in SndFile_Instrument.cpp
		/i8/p:  play
		/i8/pn:	play-with-args
*
*******************************/

// There are several main() functions with different instrument libraries and OSC address spaces

//#define CSL_OSC_FM_SndFile		// 4 voices of FM, 4 of SndFiles, and 1 bell
//#define CSL_OSC_SAMPLER			// 16 voices of file playback
//#define CSL_OSC_ADDER				// 16 voices of sum-of-sines synthesis
#define CSL_OSC_ORCHESTRA			// 16 FM, 16 file, 16 SOS

#include "Test_Support.cpp"			// include all of CSL core and the test support functions
#include "Instrument.h"
#include "BasicFMInstrument.h"
#include "SndFileInstrument.h"
#include "AdditiveInstrument.h"
#include "WhiteNoiseInstrument.h"
#include "OSC_support.h"

#ifdef CSL_MACOSX
	CAIO * theIO = new CAIO;				// create a CoreAudio IO object
#else
	PAIO * theIO = new PAIO;				// create a PortAudio IO object
#endif	

using namespace csl;

std::vector<Instrument *> library;

///////////////////////// MAIN with 4 voices of FM, 4 of SndFiles, and 1 bell

#ifdef CSL_OSC_FM_SndFile

int main(int argc, const char * argv[]) {
	READ_CSL_OPTS;									// load the standard CSL options: 
	logMsg("OSC server listening to port %s", CSL_mOSCPort);
					// Create instrument library -- 4 FM instruments and 4 sample file players
	logMsg("Setting up instrument library");
	Mixer mix(2);		// Create the main stereo output mixer

					// Now add 4 FM instruments and 4 sound file player instruments
	for (unsigned i = 0; i < 4; i++) {
		library.push_back(new BasicFMInstrument);
		mix.addInput( * library[i]);
	}
	for (unsigned i = 4; i < 6; i++) {
		library.push_back(new SndFileInstrument(CGestalt::dataFolder(), "round.snd"));
		mix.addInput( * library[i]);
	}
	for (unsigned i = 6; i < 8; i++) {
		library.push_back(new SndFileInstrument(CGestalt::dataFolder(), "wet.snd"));
		mix.addInput( * library[i]);
	}
	BasicFMInstrument * bell = new BasicFMInstrument;	// add another FM insteument set up as a bell
	bell->mAEnv.setAttack(0.001);
	bell->mAEnv.setDecay(0.001);
	bell->mAEnv.setSustain(0.2);
	bell->mAEnv.setRelease(2.9);
	bell->mIEnv.setAttack(0.001);
	bell->mIEnv.setDecay(2);
	bell->mIEnv.setSustain(0.1);
	bell->mIEnv.setRelease(0.6);
	
	library.push_back(bell);
	mix.addInput(bell);

	initOSC(UDP_PORT);		// Set up OSC address space root
							// add the instrument library OSC
	setupOSCInstrLibrary(library);

	theIO->setRoot(mix);		// plug the mixer in as the IO client
	theIO->open();			// open the IO
						
	theIO->start();			// start the io
	mainOSCLoop();			// Run the main loop function (returns on quit)
	
	theIO->stop();
	theIO->close();
} 

#endif

///////////////////////// MAIN with 16 voices of file playback

#ifdef CSL_OSC_SAMPLER

int main(int argc, const char * argv[]) {
	READ_CSL_OPTS;									// load the standard CSL options: 
	logMsg("OSC server listening to port %d", UDP_PORT);
	logMsg("Setting up sampler library");
	std::vector<Instrument *> library;
	Mixer mix(2);				// Create the main stereo output mixer
	for (unsigned i = 0; i < 16; i++) {
		library.push_back(new SndFileInstrument(CGestalt::dataFolder(), "wet.snd"));
		mix.addInput( * library[i]);
	}
	initOSC(UDP_PORT);		// Set up OSC address space root
	setupOSCInstrLibrary(library);	// add the instrument library OSC
	theIO->setRoot(mix);		// plug the mixer in as the IO client
	theIO->open();			// open the IO
	theIO->start();			// start the io
	mainOSCLoop();			// Run the main loop function (returns on quit)
	theIO->stop();
	theIO->close();
} 

#endif

///////////////////////// MAIN with 16 voices of sum-of-sines synthesis

#ifdef CSL_OSC_ADDER

int main(int argc, const char * argv[]) {
	READ_CSL_OPTS;									// load the standard CSL options: 
	logMsg("OSC server listening to port %d", UDP_PORT);
	logMsg("Setting up sampler library");
	std::vector<Instrument *> library;
	Mixer mix(2);				// Create the main stereo output mixer
	for (unsigned i = 0; i < 16; i++) {
		library.push_back(new AdditiveInstrument());
		mix.addInput( * library[i]);
	}
	initOSC(UDP_PORT);		// Set up OSC address space root
	setupOSCInstrLibrary(library);	// add the instrument library OSC
	theIO->setRoot(mix);		// plug the mixer in as the IO client
	theIO->open();			// open the IO
	theIO->start();			// start the io
	mainOSCLoop();			// Run the main loop function (returns on quit)
	theIO->stop();
	theIO->close();
} 

#endif

///////////////////////// MAIN with 16 voices each of of fancy FM, samplers, and sum-of-sines synthesis

#ifdef CSL_OSC_ORCHESTRA

int main(int argc, const char * argv[]) {
	READ_CSL_OPTS;									// load the standard CSL options: 
	logMsg("OSC server listening to port %d", UDP_PORT);
	logMsg("Setting up sampler library");
//	std::vector<Instrument *> library;
	Mixer mix(2);										// Create the main stereo output mixer
	for (unsigned i = 0; i < 16; i++) {						// 16 FMs
		library.push_back(new FancyFMInstrument);
		mix.addInput( * library[i]);
	}
	for (unsigned i = 16; i < 32; i++) {					// 16 sound files
		library.push_back(new SndFileInstrument(CGestalt::dataFolder(), "wet.snd"));
		mix.addInput( * library[i]);
	}
	for (unsigned i = 32; i < 48; i++) {					// 16 SOS
		library.push_back(new AdditiveInstrument());
		mix.addInput( * library[i]);
	}
	initOSC(UDP_PORT);			// Set up OSC address space root
	setupOSCInstrLibrary(library);	// add the instrument library OSC
	theIO->setRoot(mix);			// plug the mixer in as the IO client
	theIO->open();				// open the IO
	theIO->start();				// start the IO
	mainOSCLoop();				// Run the main loop function (returns on quit)
	theIO->stop();
	theIO->close();
} 

#endif
