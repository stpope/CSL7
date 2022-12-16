///
///  CSL_SWIG.h -- the SWIG specification file for the core classes of CSL version 6.0
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

%include exception.i 
%include "std_vector.i"
%include "std_list.i"
%include "std_map.i"
%include "std_string.i"

%module CSL6

%{
	#include <stdexcept>
	#include <vector>
	#include <list>
	#include <map>
	#include <string>
	#include "../../JuceLibraryCode/JuceHeader.h"
// Kernel
	#include "CSL_Types.h"		/// CSL type definitions and central macros, Observer classes
	#include "CSL_Exceptions.h"	/// CSL exception hierarchy
	#include "CGestalt.h"		/// System constants class
	#include "CSL_Core.h"		/// Buffer, UnitGenerator and the core classes
//Utils
	#include "RingBuffer.h"		/// Utility circular buffer
	#include "Variable.h"		/// Static/dynamic variables
	#include "BlockResizer.h"
	#include "CPoint.h"
	#include "ThreadUtilities.h"
// Sources						// no FFT for now
	#include "Window.h"
	#include "Envelope.h"		/// Breakpoint envelopes
	#include "Oscillator.h"		/// All the oscillator classes
	#include "Noise.h"
	#include "Spectral.h"
	#include "KarplusString.h"
	#include "SHARC.h"
// Processors
	#include "BinaryOp.h"		/// Binary operators
	#include "InOut.h"
	#include "Filters.h"
	#include "DelayLine.h"
	#include "Mixer.h"
	#include "Convolver.h"
	#include "FIR.h"
	#include "Freeverb.h"
// IO
	#include "JUCEIO.h"
	#include "FileIO.h"
	#include "SoundFile.h"
	#include "MIDIIOJ.h"
	#include "OSC_Support.h"
// Instruments
	#include "Accessor.h"
	#include "Instrument.h"
	#include "AdditiveInstrument.h"
	#include "FMInstrument.h"
	#include "SndFileInstrument.h"
	#include "StringInstrument.h"

	using namespace csl;
%}

////////////////////////////////

// Kernel
	%include "CSL_Types.h"		/// CSL type definitions and central macros, Observer classes
	%include "CSL_Exceptions.h"	/// CSL exception hierarchy
	%include "CGestalt.h"		/// System constants class
	%include "CSL_Core.h"		/// Buffer, UnitGenerator and the core classes
//Utils
	%include "RingBuffer.h"		/// Utility circular buffer
	%include "Variable.h"		/// Static/dynamic variables
	%include "BlockResizer.h"
	%include "CPoint.h"
	%include "ThreadUtilities.h"
// Sources						// no FFT for now
	%include "Window.h"
	%include "Envelope.h"		/// Breakpoint envelopes
	%include "Oscillator.h"		/// All the oscillator classes
	%include "Noise.h"
	%include "Spectral.h"
	%include "KarplusString.h"
	%include "SHARC.h"
// Processors
	%include "BinaryOp.h"		/// Binary operators
	%include "InOut.h"
	%include "Filters.h"
	%include "DelayLine.h"
	%include "Mixer.h"
	%include "Convolver.h"
	%include "FIR.h"
	%include "Freeverb.h"
// IO
	%include "../../JuceLibraryCode/JuceHeader.h"
	%include "/Content/Code/JUCE6/modules/juce_audio_devices/audio_io/juce_AudioIODevice.h"
	%include "JUCEIO.h"
	%include "FileIO.h"
	%include "SoundFile.h"
	%include "/Content/Code/JUCE6/modules/juce_core/memory/juce_ReferenceCountedObject.h"
	%include "MIDIIOJ.h"
	%include "OSC_Support.h"
// Instruments
	%include "Accessor.h"
	%include "Instrument.h"
	%include "AdditiveInstrument.h"
	%include "FMInstrument.h"
	%include "SndFileInstrument.h"
	%include "StringInstrument.h"

/////////////////////////////
