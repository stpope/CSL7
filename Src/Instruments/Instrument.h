///
/// Instrument.h -- The CSL pluggable instrument class.
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// 
/// Instrument are reflective objects that hold onto a DSP graph (a UnitGen), a name, and a list of "plugs"
/// (Accessors, mapped to MIDI or OSC messages) for external control.
/// Instruments can be registered with MIDI voicers or in the OSC address space.

#ifndef INCLUDE_Instrument_H
#define INCLUDE_Instrument_H

#include "CSL_Includes.h"
#include "Accessor.h"

//#define WET_DRY_MIX					// instruments support wet/dry reverb controls via global mixers
									// This is defined for the OSC server, but not for the demo app.
///
/// Keys for setParameter selector (default parameters for all notes)
///

#define set_duration_f	100			// basic accessors: dur, amp, freq, pos
#define set_amplitude_f	101
#define set_frequency_f	102
#define set_position_f	103			// position as a float or a point
#define set_position_p	104

#define set_attack_f		115			// ADSR envelope
#define set_decay_f		116
#define set_sustain_f		117
#define set_release_f		118

#define set_iattack_f		120			// 2nd ADSR envelope (index, mod)
#define set_idecay_f		121
#define set_isustain_f	122
#define set_irelease_f	123

#define set_index_f		134			// Various frequencies
#define set_c_freq_f		135
#define set_m_freq_f		136
#define set_cm_freq_r		137			// c:m freq ratio
#define set_gliss_r		138			// gliss freq ratio

#define set_vib_depth_f	140			// Vibrato, attack-chiff
#define set_chiff_amt_f	141
#define set_chiff_time_f	142

#define set_rate_f		155			// Sampler file, start, stop, rate
#define set_file_f		156
#define set_start_f		157
#define set_stop_f		158

#define set_partial_f		160			// SumOfSines partial and partial list
#define set_partials_f	161

#define wet_dry_f			170			// wet/dry reverb mix
#define set_chiff_f		171

#define rate_base_f		200			// grain cloud parameters
#define rate_range_f		201
#define offs_base_f		202
#define offs_range_f		203
#define dur_base_f		204
#define dur_range_f		205
#define dens_base_f		206
#define dens_range_f		207
#define width_base_f		208
#define width_range_f		209
#define vol_base_f		210
#define vol_range_f		211
#define env_base_f		212
#define env_range_f		213
#define gran_mode_f		214

#define ano_cmd_f			220

namespace csl  {

///
/// Instrument class (abstract) is-a UnitGenerator;
/// holds a UnitGenerator graph for its "root" and a list or envelopes to trigger;
/// adds methods for responding to MIDI and OSC to trigger notes.
///
class Instrument : public UnitGenerator {
public:
							/// Constructors
	Instrument();
	Instrument(Instrument&);								///< copy constructor
	~Instrument();
							/// Accessors
	UnitGenerator * graph() { return mGraph; };			///< my UGen graph
	UGenMap * genMap() { return & mUGens; };				///< the map of ugens in the graph by name
	UGenVector * envelopes() { return & mEnvelopes; };	///< the vector of envelopes to query or trigger

	const string name() { return mName; };				///< answer my name
														/// answer the number of channels
	UnitGenerator * genNamed(string name);				///< get a UGen from the graph

							/// Accessor management
	AccessorVector getAccessors() { return mAccessors; };				///< answer the accessor vector
	unsigned numAccessors() { return mAccessors.size(); };			///< answer the number of accessors
	virtual void setParameter(unsigned selector, int argc, void **argv, const char *types) { };	///< set a named parameter
//	virtual float getParameter(unsigned selector);

	virtual void nextBuffer(Buffer & outputBuffer) noexcept(false);	///< Sample creation

	virtual bool isActive();											///< Envelope query and re-trigger
	virtual void play();		
	virtual void playOSC(int argc, void **argv, const char *types) { };///< Play a note (subclasses refine these)
	virtual void playNote(int argc, void **argv, const char *types) { };	
	virtual void playMIDI(float dur, int chan, int key, int vel) { };	
	virtual void release();	

protected:			/// Caches
	UnitGenerator * mGraph;			///< my UGen graph
	string mName;					///< my name
	UGenMap mUGens;					///< the map of ugens in the graph by name
	UGenVector mEnvelopes;			///< the vector of envelopes to query or trigger
	AccessorVector mAccessors;		///< the accessor vector
};

}

#endif
