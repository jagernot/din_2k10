#include "din.h"
#include "console.h"
#include "solver.h"
#include "utils.h"
#include "input.h"
#include "color.h"
#include "random.h"
#include "vector2d.h"
#include "sstream"
#include "command.h"
#include "delay.h"
#include "chrono.h"
#include "osc.h"
using namespace std;

extern string dotdin; // ~/.din directory
extern console cons; // console
extern viewport view; // viewport
extern int mousex, mousey; // mouse pos
extern int LEFT, BOTTOM, RIGHT, TOP; // din board extents
extern int HEIGHT; // TOP - BOTTOM
extern float DELTA_VOLUME; // delta volume per unit key height
extern int LAST_VOLUME; // last volume in key height units
extern int NUM_VOLUMES; // number of available volumes
extern int NUM_MICROTONES; // default number of microtones in a range
extern map<char, int> NOTE_POS; // indian notation note name -> value, s - 1, r - 2, R - 3  etc
extern int SAMPLE_RATE; // sampling rate
extern map<char, float> INTERVALS; // indian notation note name -> value

din::din (cmdlist& cl) :
  aout (audio_wanted),
  win (0, 0, view.xmax, view.ymax),
	drone_master_volume (0.1),
  drone_wave ("drone.txt"),
  droneed ("drone_editor.txt"),
  dronelis (this),
	fm ("fm.txt"),
	am ("am.txt"),
	gatl ("patl.txt"),
	gatr ("patr.txt"),
  stred (this, "strength_editor.txt", "strength_editor_help.txt"),
  waved ("wave_editor.txt"),
  chaned ("channel_editor.txt"),
  strlis (this), wavelis (this),
  leftlis (this), rightlis (this),
  wavlib ("wav_lib.txt"),
  helptext ("din_help.txt"),
	gated ("gater_editor.txt"),
	gatlib ("gater_pattern_lib.txt"),
	gatllis (&gatl),
	gatrlis (&gatr),
	moded ("mod_editor.txt"),
	fmlis (&fm),
	amlis (&am),
	comed ("compressor_editor.txt"),
	coml ("coml.txt"),
	comr ("comr.txt"),
	cmdlst (cl),
	sv (this, "setv", "sv", "setv/sv <var_name> <value>"),
	gv (this, "getv", "gv", "getv/gv <var_name>"),
	sb (this, "set_bpm", "sb", "set_bpm/sb <gate/gate-l/gate-r/am/fm> <value>"),
	gb (this, "get_bpm", "gb", "get_bpm/gb <gate-l/gate-r/am/fm> <value>"),
	saa (this, "sa", "key", "sa.hlp"),
	lr (this, "load_raga", "lor", "load_raga.hlp"),
	lre (this, "load_resonators", "lre", "load_resonators/lre < <file_name> > ; eg, lre resonators"),
	sre (this, "save_resonators", "sre", "save_resonators/sre [file_name] [delete_disabled_resonators] ; eg, sre resonators 1 ; deletes disabled resonators and saves remaining resonators to file ~/.din/resonators"),
	ddr (this, "delete_disabled_resonators", "ddr", "delete_disabled_resonators/ddr ; deletes disabled resonators. no undo"),
	no (this, "notation", "no", "notation.hlp")
	
{	
		prev_mousex = prev_mousey = 0;
		
		add_commands ();
		
    current_range = 0;

		drone_sol (&drone_wave);
		droneed.add (&drone_wave, &dronelis);
		droneed.attach_library (&wavlib);
		    
    num_resonators = 0;
    cur_resonator = -1;

    waved.attach_library (&wavlib);
		gated.attach_library (&gatlib);
		
		gated.add (&gatl.crv, &gatllis);
		gated.add (&gatr.crv, &gatrlis);
		
		moded.add (&fm.crv, &fmlis);
		moded.add (&am.crv, &amlis);
		
		compress = false;
		comed.add (&coml.crv, &coml.lis);
		comed.add (&comr.crv, &comr.lis);
				    
		load_resonators (dinfo.resonators_fname);
    load_raga ();
		
		cmdlst.run ("sa");
		
		cons << eol << console::green << "Press F1 for help. You will need it!" << eol;
    		
}

void din::load_raga () {
	
  setup_ranges ();
  load_drones ();
	update_drone_tone ();
	sync_drones ();
}

bool din::load_ranges () {
	string fname = dotdin + ragainfo.name + ".ranges";
	ifstream file (fname.c_str(), ios::in);
	if (!file) {
		cout << "couldnt load range pos from " << fname << ", will use defaults " << eol;
		return false;
	}
	
	string ignore;
	file >> ignore >> ignore;
	
	int l = 0, r, w;
	for (int i = 0; i < num_ranges; ++i) {
		range& R = ranges[i];
		file >> ignore >> w;
		r = l + w;
		R.set_extents (l, BOTTOM, r, TOP);
		l = r;
	}
	
	
	
	return true;
	
}

void din::save_ranges () {
	
	string fname = ragainfo.name + ".ranges";
	ofstream file ((dotdin + fname).c_str(), ios::out);
	if (!file) {
		cout << "couldnt save range positions in " << fname << endl;
		return;
	}
	
	file << "num_ranges " << num_ranges << endl;
	for (int i = 0; i < num_ranges; ++i) {
		range& r = ranges[i];
		file << i << ' ' << r.extents.width << endl;
	}
	
}

void din::set_range_size (int ran, int sz) {
	
	if (ran == -1) {
		int r = LEFT, l;
		for (int i = 0; i < num_ranges; ++i) {
			l = r;
			r = l + sz;
			range& R = ranges[i];
			R.set_extents (l, BOTTOM, r, TOP);
		}
	} else {
		range& R = ranges[ran];
		int delta = sz - R.extents.width;
		R.set_extents (R.extents.left, BOTTOM, R.extents.left + sz, TOP);
		for (int i = ran + 1; i < num_ranges; ++i) {
			range& Ri = ranges[i];
			Ri.set_extents (Ri.extents.left + delta, Ri.extents.bottom, Ri.extents.right + delta, Ri.extents.top);
		}
	}
	
	
	
}

void din::setup_ranges () {
	
	cout << "setting up ranges..." << endl;
  
  num_ranges = ragainfo.num_octaves * ragainfo.num_ranges;
  firstr = &ranges [0];
  last_range = num_ranges - 1;
  lastr = &ranges [last_range];
	
	setup_range_notes ();
	notate_ranges ();
	
	if (load_ranges () == false) set_range_size (-1, NUM_MICROTONES);
	
	update_range_notes ();
	
	cout << "ranges setup complete." << endl;
		
}

void din::setup_range_notes () {
  for (int p = 0, r = 0; p < ragainfo.num_octaves; ++p) {
    for (int i = 0, j = 1; i < ragainfo.num_ranges; ++i, ++j) {
      range& R = ranges[r++];
      R.intervals[0] = ragainfo.notes[i];
      R.intervals[1] = ragainfo.notes[j];
    }
  }
}

void din::update_range_notes () {
  float octave_start = ragainfo.losa;
  for (int p = 0, r = 0; p < ragainfo.num_octaves; ++p) {
    for (int i = 0; i < ragainfo.num_ranges; ++i) {
			ranges[r++].calc_notes (p, octave_start, ragainfo.intervals);
    }
    octave_start *= 2;
  }
		
}

void din::range_right_changed () {

  int delta_mouse = mousex - prev_mousex;
  range& R = ranges [current_range];
  int old_right = R.extents.right;

  R.set_extents (R.extents.left, R.extents.bottom, R.extents.right + delta_mouse, R.extents.top);
	cons << cll << "num_tones = " << R.extents.width;
  int delta_right = R.extents.right - old_right;
  for (int i = current_range + 1; i < num_ranges; ++i) {
    range& ir = ranges [i];
    ir.set_extents (ir.extents.left + delta_right, ir.extents.bottom, ir.extents.right + delta_right, ir.extents.top);
  }
	
	

}

void din::range_left_changed () {

  int delta_mouse = mousex - prev_mousex;
  range& R = ranges [current_range];
  int old_left = R.extents.left;

  R.set_extents (R.extents.left + delta_mouse, R.extents.bottom, R.extents.right, R.extents.top);
	cons << cll << "num_tones = " << R.extents.width;
  int delta_left = R.extents.left - old_left;
  for (int i = 0; i < current_range; ++i) {
    range& ir = ranges [i];
    ir.set_extents (ir.extents.left + delta_left, ir.extents.bottom, ir.extents.right + delta_left, ir.extents.top);
  }
	
	

}

void din::set_key (int i) {
	key = i;
	notate_ranges ();
}

void din::set_notation (string s) {
	if (s == "w" || s == "west" || s == "western") dinfo.notate = 0;	else dinfo.notate = -1;
	notate_ranges ();
}

void din::notate_ranges () {
	
	static const char* western [] = {"C", "Db", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"};	
	if (dinfo.notate > -1) {
		for (int i = 0; i < num_ranges; ++i) {
			range& ri = ranges [i];
			char i0 = ri.intervals[0], i1 = ri.intervals[1];
			int ii0 = NOTE_POS[i0], ii1 = NOTE_POS[i1];
			int kii0 = (key + ii0) % 12;
			int kii1 = (key + ii1) % 12;
			ri.labels[0] = western [kii0];
			ri.labels[1] = western [kii1];
		}
	} else {
		for (int i = 0; i < num_ranges; ++i) {
			range& ri = ranges [i];
			char i0 = ri.intervals[0], i1 = ri.intervals[1];
			ri.labels[0] = i0;
			ri.labels[1] = i1;
		}
	}
	
}

void din::load_resonators (const string& fname) {
	
	if (fname == "") return; 
	dinfo.resonators_fname = fname;
	
	int stopped = aout.stop ();
	
	stred.clear ();
	
  ifstream file ((dotdin + dinfo.resonators_fname).c_str(), ios::in);
  if (!file) {
		num_resonators = 0;
    cur_resonator = 0;
    add_resonator ();
    cons << console::red << fname << " - bad file" << eol;
		cons << console::green << "created a resonator," << eol;
  } else {
    string ignore;
    file >> ignore >> num_resonators;
		if (num_resonators > resonator::MAX_RESONATORS) num_resonators = resonator::MAX_RESONATORS;
    file >> ignore >> cur_resonator; if (cur_resonator >= num_resonators) cur_resonator = 0;
    cout << "loading " << num_resonators << " resonators." << eol;
    for (int i = 0; i < num_resonators; ++i) {
			stringstream ss; ss << i;
			string num = ss.str();
      resonator& ri = resonators[i];
			file >> ignore >> ignore;
      file >> ignore >> ri.disabled;
      file >> ignore;
      ri.strength.load (file);
      stred.add (&ri.strength, &strlis, ri.disabled);
      file >> ignore;
      ri.wave.load (file);
      players[i].set_wave (&ri.get_wave);
      file >> ignore;
      ri.left.load (file);
      file >> ignore;
      ri.right.load (file);
      ri.update_solvers ();
			render_left_volume (i);
      render_right_volume (i);
      ri.edited = 0;
    }
    stred.pick (cur_resonator);
    cons << console::yellow << "loaded " << num_resonators << " resonators from file: " << dotdin + dinfo.resonators_fname << eol;
  }
	
	calc_multicore_vars ();
	if (stopped) aout.start ();
	
}

void din::save_resonators (int deldis) {
  ofstream file ((dotdin + dinfo.resonators_fname).c_str(), ios::out);
  if (!file) {
    cout << "cant save resonators" << eol;
  } else {
		int nr = num_resonators;
		if (deldis) for (int i = 0; i < num_resonators; ++i) if (resonators[i].disabled) --nr;
    file << "num_resonators " << nr << eol;
		clip<int> (0, cur_resonator, nr-1); file << "cur_resonator " << cur_resonator << eol;
    for (int i = 0; i < num_resonators; ++i) {
      resonator& ri = resonators[i];
      int disabled = ri.disabled;
			if (deldis && disabled); else {
				file << "resonator: " << i << eol;
				file << "disabled: " << disabled << eol;
				file << "strength:" << eol;
				ri.strength.save (file);
				file << "wave:" << eol;
				ri.wave.save (file);
				file << "left_channel:" << eol;
				ri.left.save (file);
				file << "right_channel:" << eol;
				ri.right.save (file);
			}
    }
		cons << console::yellow << "saved " << nr << " resonators to: " << dinfo.resonators_fname << eol;
  }
}

void din::set_sa (float s) {
	ragainfo.set_sa (s);
	update_range_notes ();
	update_drone_tone ();
	sync_drones ();
	mouse2sa ();
	cmdlst.run ("sa");
}

void din::mouse2sa () {
	
	// set mouse at sa
	range& r = ranges[ragainfo.num_ranges]; // range of middle sa
	int wx = r.extents.left, mx = -1;
	if (wx < win.left) {
		win.left = wx;
		win.right = win.left + view.xmax;
		mx = 0;
	} else if (wx > win.right) {
		win.right = wx;
		win.left = win.right - view.xmax;
		mx = view.xmax;
	} else mx = wx - win.left;
	
	SDL_WarpMouse (mx, mousey);
	
}

float din::get_sa () { return ragainfo.sa;}

float din::get_note_value (char c) {
	return ragainfo.intervals[c];
}

void din::retune_note (char c, float v) {
	float b4 = ragainfo.intervals[c];
	ragainfo.intervals[c] = v;
	update_range_notes ();
	update_drone_tone ();
	cons << console::green << "retuned note: " << c << " from: " << b4 << " to " << v << eol;
}

void din::retune_note () {
	
	// find nearest note
	range& r = ranges[current_range];
	int left = r.extents.left, right = r.extents.right;
	int delta_left = win_mousex - left, delta_right = right - win_mousex;
	int i = 0; if (delta_left > delta_right) i = 1;
	char label = r.intervals[i];
	
	float sas [] = {ragainfo.losa, ragainfo.sa, ragainfo.hisa};
	float sa = sas [(int) octave_position];
	float freq = step * SAMPLE_RATE;
	float interval = freq / sa;
	label = r.intervals[i];
	float b4 = ragainfo.intervals[label];
	ragainfo.intervals [label] = interval;
	update_range_notes ();
	update_drone_tone ();
	
	cons << console::green << "retuned " << label << " from " << b4 << " to " << interval << eol;
	
}

void din::restore_note () {
	
	// find nearest note
	range& r = ranges[current_range];
	int left = r.extents.left, right = r.extents.right;
	int delta_left = win_mousex - left, delta_right = right - win_mousex;
	int i = 0; if (delta_left > delta_right) i = 1;
	char n = r.intervals[i];
	if (n != 'S') {
		n = r.intervals[i];
		cons << console::green << "restored " << n << " from " << ragainfo.intervals[n] << " to " << INTERVALS[n] << eol;
		ragainfo.intervals [n] = INTERVALS [n];
		update_range_notes ();
		update_drone_tone ();
	}
}

void din::restore_all_notes () {
	ragainfo.intervals = INTERVALS;
	update_range_notes ();
	update_drone_tone ();
	cons << console::green << "restored all notes" << eol;
}

void din::save_raga () {
	save_ranges ();
	save_drones ();
	ragainfo.save_intervals ();
}

din::~din () {
	aout.stop ();
	save_raga ();
  save_resonators ();
}

void din::load_drones () {
	
  string drone_file = dotdin + ragainfo.name + ".drone";
  ifstream file (drone_file.c_str(), ios::in);
	current_drone = -1;
	num_drones = 0;
	drones.clear ();
  if (!file) return;
  else {
    string ignore;
    file >> ignore >> num_drones;
    file >> ignore >> current_drone;
    file >> ignore >> drone_master_volume;
    for (int i = 0; i < num_drones; ++i) {

      drone di;
      file >> ignore >> ignore; // ignore note name
      file >> ignore >> di.range_num;
      file >> ignore >> di.note_num;      
      file >> ignore >> di.vol;
			
      di.dv = di.vol * HEIGHT;      

      di.player.set_wave (&drone_sol);
			
      drones.push_back (di);
      
    }
		
  }
  
}

void din::save_drones () {
  drone_wave.save ("drone.txt");
  string drone_file = dotdin + ragainfo.name + ".drone";
  ofstream file (drone_file.c_str(), ios::out);
  file << "num_drones " << num_drones << eol;
  file << "current_drone " << current_drone << eol;
  file << "master_volume " << drone_master_volume << eol;
  for (int i = 0; i < num_drones; ++i) {
    drone& di = drones[i];
    file << "note_name " << ranges[di.range_num].labels[di.note_num] << eol;
    file << "range_number " << di.range_num << eol;
    file << "note_number " << di.note_num << eol;
    file << "volume " << di.vol << eol;
  }
}

void din::update_drone_tone () {
	
	for (int i = 0; i < num_drones; ++i) {
		drone& di = drones[i];
		range& r = ranges[di.range_num];
		di.step = r.notes[di.note_num].step;
		di.player.set (di.step, drone_master_volume * di.vol, drone_master_volume * di.vol);
	}
		
}

void din::sync_drones () {
	cout << "sync drones: ";
	for (int i = 0; i < num_drones; ++i) {
		cout << i << ' ';
		drone& di = drones[i];
		di.player.x = 0;
	}
	cout << endl;
}

void din::add_drone () {
  aout.stop ();
		drones.push_back (drone());
		current_drone = num_drones++;
		set_drone (current_drone);
	aout.start ();

}

void din::remove_drone () {
  
  if (current_drone == -1) return;
  aout.stop ();
		drones.erase (drones.begin() + current_drone);
		--current_drone;
		--num_drones;
		sync_drones ();
	aout.start ();
  
}

void din::set_drone (int d) {
  
    if (d == -1) return;
        
    drone& dd = drones[d];
    dd.dv = dv;
    dd.range_num = current_range;
    range& curr = ranges[current_range];
    int delta_left = win_mousex - curr.extents.left;
    int delta_right = curr.extents.right - win_mousex;
    if (delta_right < delta_left) {
    	dd.note_num = 1; 
    } else {
    	dd.note_num = 0;
    }
		
	  dd.step = curr.notes [dd.note_num].step;
    dd.vol = dv * DELTA_VOLUME;
    dd.player.set_wave (&drone_sol);
    dd.player.set (dd.step, drone_master_volume * dd.vol, drone_master_volume * dd.vol);    
		cons << "vol = " << dd.vol << eol;
		
		aout.stop ();
			sync_drones ();
		aout.start ();
      
}

void din::pick_drone () {
  
  current_drone = -1;

  const int tolerance2 = 4;
  for (int i = 0; i < num_drones; ++i) {
    drone& di = drones[i];
    int d2 = distance2 (di.xpos, 0, win_mousex, 0);
    if (d2 <= tolerance2) {
      current_drone = i;
      break;
    }
  }
  
}

void din::mark_drones () {
  
	glLineWidth (2);
  const float cc1 = 1, cc2 = 0.8;
  for (int i = 0; i < num_drones; ++i) {
    drone& di = drones[i];
    box<int>& extents = ranges[di.range_num].extents;
    if (di.note_num == 0) di.xpos = extents.left; else di.xpos = extents.right;
    int x = di.xpos, y = BOTTOM, ydv = y + di.dv;
    if (i == current_drone) glColor3f (cc2, cc1, cc2); else glColor3f (cc2, cc2, cc2);
    glBegin (GL_LINE_LOOP);
      glVertex2i (x - di.dv, ydv);
      glVertex2i (x, y);
      glVertex2i (x + di.dv, ydv);
    glEnd ();
  }
	glLineWidth (1);
}

void din::update_drone_master_volume (float d) {
	drone_master_volume += d;
	if (drone_master_volume < 0) drone_master_volume = d;
	for (int i = 0, j = drones.size(); i < j; ++i) {
		drone& di = drones[i];
		di.player.set_volume (drone_master_volume * di.vol, drone_master_volume * di.vol);
	}
	cons << cll << "drone master volume = " << drone_master_volume;
	
}

bool din::handle_input () {

  if (keypressed (SDLK_F1)) helptext();
      
  // movement
  if (keydown ('a')) scroll (-dinfo.scroll.x);
  if (keydown ('d')) scroll (dinfo.scroll.x);
  if (keydown ('w')) win (win.left, win.bottom + dinfo.scroll.y, win.right, win.top + dinfo.scroll.y);
  if (keydown ('s')) win (win.left, win.bottom - dinfo.scroll.y, win.right, win.top - dinfo.scroll.y);

	// mouse pos in win
	// win = view so there is very minimal and therefore fast view2win transform
	win_mousex = win.left + mousex;
	win_mousey = win.bottom + view.ymax - mousey;
		
  //
  // phrasor
  //  
  if (phrasor0.state == phrasor::recording) {
		static phrase_data pd;
		pd.left = win.left; pd.bottom = win.bottom;
		pd.right = win.right; pd.top = win.top;
		pd.win_mousex = win_mousex; pd.win_mousey = win_mousey;
		pd.mousex = mousex; pd.mousey = mousey;
		phrasor0.data.push_back (pd);
	}
  
  if (keypressed ('f')) {
    cons << console::yellow;
    if (phrasor0.state == phrasor::recording) {
			phrasor0.validate ();
			phrasor0.state = phrasor::playing;
      cons << "phrasor STOPPED recording." << eol;
    } else {
      phrasor0.state = phrasor::recording;
      cons << "phrasor is RECORDING." << eol;
    }
  }
  
  if (keypressed ('v')) {
    if (phrasor0.state == phrasor::playing) {
      cons << console::red << "phrasor has PAUSED." << eol;
      phrasor0.state = phrasor::paused;
    } else {
      cons << console::green << "phrasor is PLAYING." << eol;
			phrasor0.validate ();
      phrasor0.play ();
    }
  }
  
  if (keypressed ('g')) {
    phrasor0.clear ();
    cons << console::red << "phrases deleted." << eol;
  }
  
  // dj controls
  jogged = 0;
  if (keydown (SDLK_LEFT)) {phrasor0.jog (-3); jogged = 1;}
  if (keydown (SDLK_RIGHT)) {phrasor0.jog (+3); jogged = 1;}
  if (keydown (SDLK_DOWN)) phrasor0.set_cue ();
  if (keydown (SDLK_UP)) phrasor0.goto_cue (); 
  
  
  // oscilloscope
  if (keypressed (SDLK_RALT)) osc.toggle_visible ();
  if (keypressed (SDLK_MENU)) osc.toggle_pause ();
  
	// drone
	if (keypressed ('q')) add_drone ();
	if (keypressed ('e')) {
		pick_drone ();
		set_drone (current_drone);
	}
	
	if (keypressed ('c')) {
		pick_drone ();
		remove_drone ();
	}
	
	if (keypressed ('o')) {
		extern oserver srv;
		srv.toggle ();
		if (srv.enabled) cons << console::green << "server enabled" << eol; else cons << console::yellow << "server paused" << eol;
	}
	
	if (keypressed ('p')) {
		compress = !compress;
		if (compress) cons << console::green << "compressor enabled." << eol; else cons << console::red << "compressor disabled." << eol;
	}
		
	static const float delta_drone_master = 0.01;
	if (keypressedd (',')) update_drone_master_volume (-delta_drone_master);
	if (keypressedd ('.')) update_drone_master_volume (+delta_drone_master);
	
	static float last_drone_master_volume = 0;
	if (keypressedd ('/')) {
		if (drone_master_volume == 0) update_drone_master_volume (last_drone_master_volume); else {
			last_drone_master_volume = drone_master_volume;
			update_drone_master_volume (-drone_master_volume);
		}
	}
	
	if (keypressed ('\'')) {
		dinfo.delay = !dinfo.delay;
		cons << "delay ";
		if (dinfo.delay) cons << "enabled." << eol; else cons << " disabled." << eol;
	}
	
	if (keypressed (';')) {
		set_sa (step * SAMPLE_RATE);
	}
		        
  if (keydown (SDLK_LCTRL)) {
      range_left_changed ();
  }
  
  if (keydown (SDLK_LSHIFT)) {
      range_right_changed ();
  }
    
  if (keypressed ('b')) {
		!gatl; if (gatl.enabled) cons << console::green << "gate-l enabled" << eol; else cons << console::red << "gate-l disabled" << eol;
		!gatr; if (gatr.enabled) cons << console::green << "gate-r enabled" << eol; else cons << console::red << "gate-r disabled" << eol;
	}
		
	if (keypressed ('n')) {
		if (keydown (SDLK_LSHIFT)) restore_note (); else 
		if (keydown (SDLK_LCTRL)) restore_all_notes(); else 
		retune_note ();
	}
        
	if (keypressed ('m')) {
		if (keydown (SDLK_LSHIFT)) {
			set_range_size (-1, NUM_MICROTONES); 
		} else if (keydown (SDLK_LCTRL)) {
			set_range_size (-1, ranges[current_range].extents.width);
		} else set_range_size (current_range, NUM_MICROTONES);
	}
	
	if (keypressed ('i')) {
		extern oserver srv;
		srv.msg = !srv.msg;
		if (srv.msg) cons << console::green << "mesgs on" << eol; else cons << console::red << "mesgs off" << eol;
	}
	
	// change board height
	static int mod_dh = 0;
	if (keydown ('h')) {
		if (mod_dh == 0) {
			mod_dh = 1;
		} else {
			int dy = mousey - prev_mousey;
			extern int MAX_HEIGHT;
			HEIGHT -= dy;
			clip<int> (1, HEIGHT, MAX_HEIGHT);
			TOP = BOTTOM + HEIGHT;
			for (int i = 0; i < num_ranges; ++i) {
				ranges[i].extents.top = TOP;
				ranges[i].extents.height = HEIGHT;
			}
			
			calc_volume_vars (HEIGHT);
			
			for (int i = 0; i < num_drones; ++i) {
				drone& di = drones[i];
				di.dv = di.vol * HEIGHT;
			}
			
			cons << cll << console::yellow << "board height = " << HEIGHT;
			
		}
	} else {
		if (mod_dh) {
			mod_dh = 0;
			render_volumes ();
			cons << eol;
		}
	}
	
  prev_mousex = mousex;
	prev_mousey = mousey;
	
  return true;

}

void din::scroll (int dx) {
	mousex -= dx;
	win (win.left + dx, win.bottom, win.right + dx, win.top);
	SDL_WarpMouse (mousex, mousey);
	prev_mousex -= dx;
}

void din::find_current_range () {
  
  current_range = 0;
  
  int moux = win.left + mousex;

  for (int i = 0; i < num_ranges; ++i) {
    range& curr = ranges[i];
    box<int>& ext = curr.extents;
    if ( (moux >= ext.left) && (moux <= ext.right)) {
      current_range = i;
      break;
    }
  }
    
}

bool din::find_tone_and_volume () {		 
	
	// cursor height on din keyboard.	
	dv = win_mousey - BOTTOM; 
	
	if (dv > LAST_VOLUME) dv = LAST_VOLUME;
	
	// current tone
	range* curr = &ranges [current_range];
	int deltax = win_mousex - curr->extents.left;
	if (deltax >= curr->extents.width) {
		++current_range;
		if (current_range == num_ranges) {
			current_range = last_range;
			curr = lastr;
		} else {
			curr = &ranges [current_range];
		}
	} else if (deltax < 0) {
		--current_range;
		if (current_range < 0) {
			curr = firstr;
			current_range = 0;
		} else {
			curr = &ranges [current_range];
		}
	} 
	
	deltax = win_mousex - curr->extents.left;
	delta = deltax * curr->extents.width_1;
	step = curr->notes[0].step + delta * curr->delta_step; // current tone
	octave_position = curr->notes[0].octave_position + delta * curr->delta_octave_position; 
	  
  if (dv < 0) { // below keyboard, silence
    for (int i = 0; i < num_resonators; ++i) {
			play& p = players[i];
			p.set (step, 0, 0);
			p.x = 0;
			am_vol = 0;
			gatl.now = gatr.now = 0;
			fm.now = am.now = 0;
		}
		return false;
  } else {
		am_vol = dv * DELTA_VOLUME;
    for (int i = 0; i < num_resonators; ++i) {
      play& pi = players[i];
      if (resonators[i].disabled == false) {
        float next_left = volumes[dv].res[i].left;
        float next_right = volumes[dv].res[i].right;
        float next_strength = resonators[i].get_strength (octave_position);
        pi.set (step, next_left * next_strength, next_right * next_strength);
      } else {
				pi.set (step, 0, 0);
				pi.x = 0;
			}
    }
		return true;
  }
    
}

void din::render_left_volume (int r) {

  resonator& rr = resonators[r];
  solver gety (&rr.left); gety.init ();

  float x = DELTA_VOLUME;
  for (int i = 0; i < NUM_VOLUMES; ++i) {
    volumes[i].res[r].left = gety (x);
    x += DELTA_VOLUME;
  }

}

void din::render_right_volume (int r) {

  resonator& rr = resonators[r];
  solver gety (&rr.right); gety.init ();

  float x = DELTA_VOLUME;
  for (int i = 0; i < NUM_VOLUMES; ++i) {
    volumes[i].res[r].right = gety (x);
    x += DELTA_VOLUME;
  }

}

void din::render_volumes () {
  for (int i = 0; i < num_resonators; ++i) {
    render_left_volume (i);
    render_right_volume (i);
  }
}

void din::add_resonator () {
  if (num_resonators < resonator::MAX_RESONATORS) {
    
    resonator& ri = resonators[num_resonators];
    
    ri.strength.load ("default_map.txt");
    ri.get_strength (&ri.strength);
    
    ri.wave.load ("default_wave.txt");
    ri.get_wave (&ri.wave);
        
    players[num_resonators].set_wave (&ri.get_wave);
    
    ri.left.load ("default_left.txt");
    ri.right.load ("default_right.txt");
    
    stred.add (&ri.strength, &strlis, false);
    ri.update_solvers ();
    
    render_left_volume (num_resonators);
    render_right_volume (num_resonators);
		
		calc_multicore_vars ();
    
    ++num_resonators;
    
    cons << "added resonator "<< eol;
    
  } else {
    cons << "sorry, cant add resonator. change resonator::MAX_RESONATORS for more." << eol;
    return;
  }
	
}

void din::edit_resonator (int i, unsigned int what) {
  resonator& ri = resonators[i];
  switch (what) {
    case resonator::STRENGTH:
      //ri.get_strength.reinit = true;
      //cout << "edited strength of " << i << eol;
			ri.get_strength.init ();
    break;
    case resonator::WAVE:
      //cout << "edited wave of " << i << eol;
      //ri.get_wave.reinit = true;
			ri.get_wave.init ();
    break;
    case resonator::LEFT:
      render_left_volume(i);
      //cout << "edited left of " << i << eol;
    break;
    case resonator::RIGHT:
      render_right_volume(i);
      //cout << "edited right of " << i << eol;
    break;
  }
}

void din::select_resonator (int sel) {
  
    if (sel < 0 || sel >= num_resonators) return;

    cur_resonator = sel;
    resonator& rs = resonators[sel];
    waved.clear ();
    waved.add (&rs.wave, &wavelis);
		waved.dofft ();
    chaned.clear ();
    chaned.add (&rs.left, &leftlis);
    chaned.add (&rs.right, &rightlis);
    stred.select_curve (sel);
}

void din::browse_resonator (int i) {
	cur_resonator += i;
	if (cur_resonator > -1 && cur_resonator < num_resonators) stred.select_curve (cur_resonator);
	cons << console::cyan << "resonator " << cur_resonator << eol;
}

void din::disable_resonator (int i, bool dis) {
  if (i > -1 && i < resonator::MAX_RESONATORS) {
    resonator& ri = resonators[i];
    ri.disabled = dis;
    ri.edited |= resonator::STRENGTH;
    /*cons << "resonator " << i << " is ";
    if (dis == false) cons << console::green << " enabled." << eol; else cons << console::red << " disabled." << eol;*/
  }
}

void din::disable_all_resonators () {
	for (int i = 0; i < num_resonators; ++i) {
		disable_resonator (i);
	}
}

void din::draw () {

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho (win.left, win.right, win.bottom, win.top, -1, 1);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
	  
  if (osc.visible) {
    extern int BOTTOM;
    glTranslatef (win.left, BOTTOM - oscilloscope::MAX_HEIGHT, 0);
    osc.draw ();
    glLoadIdentity ();
  }
  
  //
  // draw ranges
  //
				
  int rl = firstr->extents.left, rr = lastr->extents.right;
	float tr = 0.8; glColor3f (tr, tr, tr);
  glBegin (GL_LINES);
    glVertex2i (rl, BOTTOM);
    glVertex2i (rr, BOTTOM);
    glVertex2i (rl, TOP);
    glVertex2i (rr, TOP);
  glEnd ();
  
  // label the ranges
  for (int i = 0; i < last_range; ++i) ranges[i].draw_labels ();
  lastr->draw_labels (range::BOTH);
	
	mark_drones ();  
	
	if (phrasor0.state == phrasor::paused) phrasor0.draw ();  

}

void din::enter () {
	
	SDL_WarpMouse (prev_mousex, prev_mousey);
	mousex = prev_mousex;
	mousey = prev_mousey;
  find_current_range ();
  find_tone_and_volume ();  
  phrasor0.warpmouse = true;
  if (phrasor0.state != phrasor::playing) aout.start ();
}

void din::leave () {
  phrasor0.warpmouse = false;
}

void din::change_depth (int i, float d) {
  if (i == 1) {
    fm_depth += d;
    hz2step (fm_depth, fm_step);
    cons << cll << "fm depth: " << fm_depth;
  } else {
    am_depth += d;
    cons << cll << "am depth: " << am_depth;
  }
}

void din::fill () {
	
	am.apply_modulation (ams, audio_out::CHANNEL_BUFFER_SIZE, am_depth * am_vol);
	fm.apply_modulation (fms, audio_out::CHANNEL_BUFFER_SIZE, fm_step);
	  
}

void din::bg () {
  
  if (phrasor0.state == phrasor::playing) {
    extern int mousex, mousey;
    if (phrasor0.warpmouse) {
      phrasor0.get (win, mousex, mousey, win_mousex, win_mousey);
      SDL_WarpMouse (mousex, mousey);
    } else phrasor0.get (win_mousex, win_mousey);
    if (!jogged) phrasor0.next ();
  }
  
}

void din::add_commands () {
	cmdlst.add (&sv);
	cmdlst.add (&gv);
	cmdlst.add (&sb);
	cmdlst.add (&gb);
	cmdlst.add (&saa);
	cmdlst.add (&lr);
	cmdlst.add (&lre);
	cmdlst.add (&sre);
	cmdlst.add (&ddr);
	cmdlst.add (&no);
}

din_info::din_info () {

	extern string dotdin;
	ifstream file ((dotdin + "din_info").c_str(), ios::in);
	string ignore;
	if (file) {
		file >> ignore >> resonators_fname;
		file >> ignore >> scroll.x >> scroll.y;
		file >> ignore >> height; HEIGHT = height; TOP = BOTTOM + HEIGHT; calc_volume_vars (HEIGHT);
		file >> ignore >> notate;
		file >> ignore >> cons.rollup;
		file >> ignore >> ncores;
		file >> ignore >> delay;
	} else {
		resonators_fname = "one";
		scroll = point<int>(1, 1);
		height = 300;
		notate = -1;
		cons.rollup = false;
		ncores = 1;
		delay = 1;
	}
	cons.last ();
}

din_info::~din_info () {
	extern string dotdin;
	ofstream file ((dotdin + "din_info").c_str(), ios::out);
	if (file) {
		file << "resonators_file " << resonators_fname << endl;
		file << "scroll " << scroll.x << ' ' << scroll.y << endl;
		file << "board_height " << HEIGHT << endl;
		file << "notate " << notate << endl;
		file << "rollup " << cons.rollup << endl;
		file << "ncores " << ncores << endl;
		file << "delay " << delay << endl;
	} else {
		cout << "cannot save din_info" << eol;
	}
}

void din::calc_multicore_vars () {
	dinfo.resonators_per_core = num_resonators * 1./ dinfo.ncores + 1;
	dinfo.last_resonator = num_resonators - 1;
}
