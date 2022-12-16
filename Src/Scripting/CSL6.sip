// Define the SIP wrapper to the CSL library.

%Module(name=CSL6, language="C")

%MethodCode

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

%End

