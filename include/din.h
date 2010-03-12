#ifndef DIN
#define DIN

#include "curve_editor.h"
#include "strength_editor.h"
#include "listeners.h"
#include "range.h"
#include "resonator.h"
#include "modder.h"
#include "volume.h"
#include "ui.h"
#include "audio.h"
#include "play.h"
#include "oscilloscope.h"
#include "curve_library.h"
#include "beat2value.h"
#include "help.h"
#include "phrasor.h"
#include "input.h"
#include "raga_info.h"
#include "command.h"
#include "compressor.h"
#include "globals.h"

using namespace std;

struct drone { // drone tone

  float step; // determines frequency
  float vol; // actual volume
  play player; // player for this drone

	int range_num; // range number
	int note_num; // note number
	
  int xpos; // for drawing drone marker and picking
  int dv; // driver volume
	  
};

struct din_info {
	
	string resonators_fname; // resonators file
	
	point<int> scroll; // x and y scroll speed
	int height; // din board height
	
	int notate; // display notation for ranges (0 - western and 1 - indian)
	
	int delay; // is delay enabled?
	
	// multicore vars
	int ncores;
	int resonators_per_core;
	int last_resonator;
	
	din_info ();
	~din_info ();
	
};

struct din : public ui {
	
	//
	// portaudio
	//
	paudio paud;
	audio_out aout;
	
  int num_resonators;
  play players[resonator::MAX_RESONATORS]; // player assigned for each resonator

  // stereo volume of all resonators
	static const int MAX_VOLUMES = 8192;
  volume volumes [MAX_VOLUMES];

  // tone ranges
	static const int MAX_RANGES = 64;
	range ranges [MAX_RANGES];
  int num_ranges, last_range, current_range;
  range *firstr, *lastr;
	float step, delta, octave_position;
	
  void setup_ranges ();
	void setup_range_notes ();
	void update_range_notes ();
	void range_right_changed ();
	void range_left_changed ();
	bool load_ranges ();
	void set_range_size (int r, int sz);
	void save_ranges ();
	void find_current_range ();
	bool find_tone_and_volume ();
	void retune_note (char n, float v);
	void retune_note ();
	void restore_note ();
	void restore_all_notes ();
	float get_note_value (char n);
	
  // mouse board
  int win_mousex, win_mousey; // mouse pos in win
  int prev_mousex, prev_mousey;
  int dv; // driver volume ie mouse height on board
  note* K; // current key data
	box<int> win;
  void scroll (int dx);

	void set_sa (float sa);
	void mouse2sa ();
	float get_sa ();

	
	//
	// drones
	//
  vector<drone> drones;
  int num_drones;
  float drone_master_volume;
	
  multi_curve drone_wave; // drone waveform
  solver drone_sol; // solver for drone waveform
  curve_editor droneed; // drone waveform editor
  drone_listener dronelis; // handles drone edits
  int current_drone;
	void load_drones ();
	void save_drones ();
	void sync_drones ();
	void add_drone ();
	void set_drone (int d);
	void pick_drone ();
	void remove_drone ();
	void mark_drones ();
	void update_drone_tone ();
	void update_drone_master_volume (float delta);

	
	// am & fm
	float am_depth, am_vol;
	float fm_depth, fm_step;
	float ams [audio_out::CHANNEL_BUFFER_SIZE], fms [audio_out::CHANNEL_BUFFER_SIZE];
	beat2value fm, am;
  beat2value gatl, gatr;
	
	void change_depth (int i, float d);
	void fill ();

  //
  // resonators
  //
  resonator resonators [resonator::MAX_RESONATORS];
  int cur_resonator;
	void add_resonator ();
	void edit_resonator (int i, unsigned int what);
	void select_resonator (int sel);
	void disable_resonator (int i, bool dis = true);
	void disable_all_resonators ();
	void render_left_volume (int r);
	void render_right_volume (int r);
	void render_volumes ();
	void render_resonator_strengths (range& ran);
	void load_resonators (const string& fname);
	void save_resonators (int deldis = 0);
	void browse_resonator (int i);
	void only_enable_resonator (int i);
	void enable_all_resonators ();

  // resonator editors
  strength_editor stred; // resonator.strength
  curve_editor waved; // resonator.wave
  curve_editor chaned; // edits both resonator.left and resonator.right

  // listeners interested in resonator edits
  strength_listener strlis;
  wave_listener wavelis;
  left_listener leftlis;
  right_listener rightlis;
  
  curve_library wavlib; // waveform library 

  raga_info ragainfo;
	
	// range display notation
	int notate; // -1 = indian, 0 = western, 1 = numeric
	void set_notation (string s);
	void notate_ranges ();
	
	// musical key
	int key;
	void set_key (int i);
	int get_key () { return key;}
	
	// phrasor - record/play/scratch a phrase
	phrasor phrasor0;
	int jogged;
  
  oscilloscope osc;
		  
  // help
  help helptext;
  
	// beat2value editors  
	curve_editor gated;
	curve_library gatlib;
	beat2value_listener gatllis, gatrlis;
  
	// am & fm modulation editors
	curve_editor moded;
	beat2value_listener fmlis, amlis;
	
	curve_editor comed;
	compressor coml, comr;
	bool compress;
	
	din (cmdlist& cmdlst);
	~din ();
	
	bool handle_input ();
		
	void load_raga ();
	void save_raga ();

	void enter ();
	void leave ();
	void bg (); // background execution
	void draw ();
		
	// commands
	cmdlist& cmdlst;
	setv sv;
	getv gv;
	set_bpm sb;
	get_bpm gb;
	sa saa;
	load__raga lr;
	load__resonators lre;
	save__resonators sre;
	delete_disabled_resonators ddr;
	notation no;
	void add_commands ();
	
	din_info dinfo;
	
	void calc_multicore_vars ();
	
};

#endif
