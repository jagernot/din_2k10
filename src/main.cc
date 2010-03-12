/*
 * din
 * copyright(c) 2006-2009 Sampath Jagannathan
 * distributed under GPL 2.0 license.
 * 
*/
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <portaudio.h>
#include "box.h"
#include "utils.h"
#include "font.h"
#include "console.h"
#include "input.h"
#include "viewwin.h"
#include "curve.h"
#include "multi_curve.h"
#include "solver.h"
#include "chrono.h"
#include "ui.h"
#include "curve_editor.h"
#include "font_editor.h"
#include "din.h"
#include "curve_library.h"
#include "console_iterator.h"
#include "delay.h"
#include "random.h"
#include "globals.h"
#include "command.h"
#include "ragalist.h"
#include "fft.h"
#include "sine_mixer.h"
#include "checkdotdin.h"
#include "osc.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <algorithm>
using namespace std;

string dotdin; 
checkdotdin cdd;

font fnt ("jag.fnt");

string APP_NAME = "din";
int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480, SCREEN_DEPTH = 16;

int SAMPLE_RATE = 48000;

string INTERVALS_FILE = "ji";
map <char, float> INTERVALS;

map <char, int> NOTE_POS;
int NUM_INTERVALS;
char FIRST_INTERVAL_NAME, LAST_INTERVAL_NAME;

int NUM_VOLUMES = 512;

int LAST_VOLUME;
float DELTA_VOLUME = 1./NUM_VOLUMES;

string RAGA = "malkauns";
int NUM_MICROTONES = 100;

int MAX_HEIGHT = din::MAX_VOLUMES - 1;
int HEIGHT = min (320, MAX_HEIGHT); 
int LEFT = 0, BOTTOM = 250, TOP = BOTTOM + HEIGHT;


viewport view;
load_globals lg;

ragalist ragalst;

keyboard keybd;
int mousex, mousey;

console cons;

delay left_delay (500, 0.7, 0.3), right_delay (500, 0.7, 0.3);

extern int SAMPLE_RATE;
chrono clk (SAMPLE_RATE);

fft fft0;
sine_mixer sinemixer;

cmdlist cmdlst;

din dn (cmdlst);

show_help help ("help", "help", "help <command_name>", &cmdlst);
set_delay sd (&left_delay, &right_delay, "set_delay", "sd", "set_delay.hlp");
get_delay gd (&left_delay, &right_delay, "get_delay", "gd", "get_delay/gd <left/right>");
add_raga ar ("add_raga", "ar", "add_raga.hlp");
ls_raga lsr ("ls_raga", "lsr", "list_raga.hlp");
remove_raga rr ("remove_raga", "rr", "remove_raga.hlp");
note_distance nd ("note_distance", "nd", "note_distance <note1> <note2>; nd C G");
chord ch ("chord", "ch", "chord.hlp");
set_kern sk ("set_kern", "sk", "set_kern <chara> <charb> <amt>; eg., sk C a -1");
get_kern gk ("get_kern", "gk", "get_kern <chara> <charb>; eg., gk i j");
set_font_size sfs ("set_font_size", "sfs", "set_font_size <cellsz> <charspc>; eg., sfs 2 1;");
listv lv ("listv", "lv", "listv/lv - lists all vars");

curve_name cn ("curve_name", "cn", "curve_name <name>; set picked curve name");

lo_address client  = 0;
oconnect co (client, "connect", "co", "connect <ip> <port>; opens an osc connection on ip at port. eg. co 83.22.199.1 3333");
osend snd (client, "send", "snd", "send <path> <format> <data>");
otransmit trn (client, "transmit", "trn", "transmit <path> <ntimes> <nsecs>");

mouse_sender ms (&dn, client, "/mouse", "ii");
pitch_sender ps (&dn, client, "/pitch", "f");
volume_sender vs (&dn, client, "/volume", "f");
win_sender wsr (&dn, client, "/window", "iiii");
sa_sender ssr (&dn, client, "/key", "f");

int MAX_VARS = 5;

sender* sndr [] = {
	&ms,
	&ps,
	&vs,
	&wsr,
	&ssr
};

string vars [] = {
	ms.oo.path,
	ps.oo.path,
	vs.oo.path,
	wsr.oo.path,
	ssr.oo.path
};

timer tmrs [5];

oserver srv ("oserver", "re", "oserver/re <port> <cmd_timeout_msecs>");
oid ido ("oid", "id", "oid/id <name>");

echo ech ("echo", "says", "echo/says <string>. prints string on the console");

set_pitch_and_volume spv (&dn, "set_pitch_and_volume", "spv", "set_pitch_and_volume <pitch> <volume>");

struct work_t {
	int start, end, fpb;
	float buf [audio_out::STEREO_BUFFER_SIZE];
};

const int MAX_CORES = 8;
work_t work [MAX_CORES];
pthread_t tid [MAX_CORES];

void* dowork (void* arg) {
	
	work_t* w = (work_t*) arg;
	
  memset (w->buf, 0, audio_out::NUM_BUFFER_BYTES); 
	
	int start = w->start, end = w->end, fpb = w->fpb;
	float* buf = w->buf;
	for (int i = start; i < end; ++i) {
		float* buf1 = buf;
		dn.players[i] (buf1, fpb, dn.ams, dn.fms);
	}
	
	return 0;
	
}

void addcmds () {
	cmdlst.add (&help);
	cmdlst.add (&sd);
	cmdlst.add (&gd);
	cmdlst.add (&ar);
	cmdlst.add (&lsr);
	cmdlst.add (&rr);
	cmdlst.add (&nd);
	cmdlst.add (&ch);
	cmdlst.add (&sk);
	cmdlst.add (&gk);
	cmdlst.add (&sfs);
	cmdlst.add (&lv);
	cmdlst.add (&co);
	cmdlst.add (&trn);
	cmdlst.add (&snd);
	cmdlst.add (&srv);
	cmdlst.add (&ido);
	cmdlst.add (&ech);
	cmdlst.add (&spv);
	cmdlst.add (&cn);
}


uis_t::uis_t () /* :fed (fnt, "font_editor.txt")*/ {

	last = current = 0;
	
	uis.push_back (&dn);
	uis.push_back (&dn.stred);
	uis.push_back (&dn.waved);
	uis.push_back (&dn.chaned);
	uis.push_back (&dn.moded);
	uis.push_back (&dn.gated);
	uis.push_back (&dn.comed);
	uis.push_back (&dn.droneed);
	
	set_current (&dn, 0);

}

void uis_t::set_current (ui* u, int crved) {
	if (current) current->leave ();
	last = current;
	current = u;
	if (crved) current_curve_editor = dynamic_cast<curve_editor*>(u);
	current->enter ();
}

bool uis_t::handle_input () {
	
	for (int i = 0, j = uis.size(); i < j; ++i) uis[i]->bg (); // bg processing
	
	cons.handle_input ();
	
	if (cons.command_mode == 0) {
		
		current->handle_input ();
		
		static int all = 0;
		if (keypressed (SDLK_F2)) {
			if (--dn.cur_resonator < 0) dn.cur_resonator = dn.num_resonators - 1;
			hit_t hit (&dn.resonators[dn.cur_resonator].strength, dn.cur_resonator);
			dn.stred.pick (hit.crv_id);
			dn.stred.enable (hit);
			cons << console::green << "active resonator: " << dn.cur_resonator << eol;
			all = 1;
		}
		
		
		if (keypressed (SDLK_F3)) {
			if (++dn.cur_resonator >= dn.num_resonators) dn.cur_resonator = 0;
			hit_t hit (&dn.resonators[dn.cur_resonator].strength, dn.cur_resonator);
			dn.stred.pick (hit.crv_id);
			dn.stred.enable (hit);
			cons << console::green << "active resonator: " << dn.cur_resonator << eol;
			all = 1;
		}
		
		if (keypressed (SDLK_F4)) {
			if (all) {
				dn.stred.enable_all ();
				all = 0;
				cons << console::green << "all resonators enabled." << eol;
			} else {
				hit_t hit (&dn.resonators[dn.cur_resonator].strength, dn.cur_resonator);
				dn.stred.pick (hit.crv_id);
				dn.stred.enable (hit);
				cons << console::yellow << "active resonator " << dn.cur_resonator << eol;
				all = 1;
			}
			
		}
	
		if (keypressed ('1')) set_current (&dn.stred);
		if (keypressed ('2')) set_current (&dn.waved);
		if (keypressed ('3')) set_current (&dn.chaned);
		if (keypressed ('4')) set_current (&dn.moded);
		if (keypressed ('5')) set_current (&dn.gated);
		if (keypressed ('6')) set_current (&dn.comed);
		
		
		const float DELTA_DEPTH [2] = {0.01, 1};
		static int MOD_MODE = 0;
		if (keypressed (SDLK_F10)) {
			MOD_MODE = !MOD_MODE;
			if (MOD_MODE) cons << console::yellow << "change FM depth = " << dn.fm_depth << eol;
			else cons << console::yellow << "change AM depth = " << dn.am_depth << eol;
		}
		if (keypressedd (SDLK_F11)) dn.change_depth (MOD_MODE, -DELTA_DEPTH[MOD_MODE]);
		if (keypressedd (SDLK_F12)) dn.change_depth (MOD_MODE, DELTA_DEPTH[MOD_MODE]);
		
		/*if (keypressed ('7')) set_current (&fed);
		if (keypressed ('8')) { cons << console::red << "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ 1234567890 <>:;,'`#[]%+-*()" << eol; cons << console::yellow << "the quick brown fox jumps over the lazy dog" << eol;set_current (&fed);}*/
				
	}
		
	if (keypressed (SDLK_ESCAPE)) {
		if (current == &dn) return false; else set_current (&dn);
	}
		

	return true;
}

void uis_t::draw () {
	glClear (GL_COLOR_BUFFER_BIT);
		current->draw ();
		cons.draw ();
	SDL_GL_SwapBuffers ();
}

uis_t uis;

void set_window (int w, int h) {
	extern string APP_NAME;
	extern int SCREEN_DEPTH;
	view = viewport (APP_NAME, w, h, SCREEN_DEPTH);
	fft0.lev.chkpos ();
	sinemixer.sine_levels.chkpos ();
	--w; --h;
	cons.set_window (box<int>(0, -h, w, 0));
	dn.win (0, 0, w, h);
	dn.phrasor0.clear ();
	
	SDL_WarpMouse (0, h);
}

void load_window () {

	extern int SCREEN_WIDTH, SCREEN_HEIGHT;
	int width = SCREEN_WIDTH, height = SCREEN_HEIGHT;
	string ignore;

	extern string dotdin;
	ifstream file ((dotdin + "window").c_str(), ios::in);
	if (file) {
		file >> ignore >> width;
		file >> ignore >> height;
	}
	set_window (width, height);
}

void save_window () {
	const char* fname = "window";
	extern string dotdin;
	ofstream file ((dotdin + fname).c_str(), ios::out);
	extern viewport view;
	if (file) {
		file << "width " << view.width << endl;
		file << "height " << view.height << endl;
	}	
}

bool read_input () {
	
    SDL_Event event;
    while (SDL_PollEvent(&event)); {
      switch(event.type) {
        case SDL_VIDEORESIZE:
          try {
            set_window (event.resize.w, event.resize.h);
          } catch (const badviewport& bv) {
            cout << "sdl: viewport init failed" << endl;
            return false;
          }
          break;
      }
    }
		
    keybd.read ();
    SDL_GetMouseState (&mousex, &mousey);

    return true;

 }

int main (int argc, char** argv) {
	system (". ~/.din/m00"); // xset m 0 0 for pure mouse data, without any fudge applied by X.
  setSeed (clock()); 
  addcmds ();
  cons.home ();
  load_window ();
  while (1) {
    if (!read_input() || !uis.handle_input ()) break;
    uis.draw ();
		osc ();
    keybd.save ();
  }
	save_window ();
	system ("xset m $ACCEL $THRESHOLD"); // restore mouse settings
  return 0;
}

int audio_wanted (const void* ib, void* ob, unsigned long fpb,
									const PaStreamCallbackTimeInfo* tinfo,
									PaStreamCallbackFlags flags,
									void *data)
{
  
  float* out0 = (float *) ob;
	  
  memset (out0, 0, audio_out::NUM_BUFFER_BYTES); 
	
	// no am/fm
	/*if (dn.find_tone_and_volume ()) {
		
		for (int i = 0; i < dn.num_resonators; ++i) {
			float* rout = out0;
			dn.players[i] (rout, fpb);
		}*/
		
		/*int nthreads = dn.dinfo.ncores;
		for (int i = 0, j = 0; i < nthreads; ++i) {
			work_t& wi = work[i];
			wi.start = j;
			wi.end = min (wi.start + dn.dinfo.resonators_per_core, dn.dinfo.last_resonator);
			j = wi.end;
			wi.fpb = fpb;
			pthread_create (&tid[i], 0, dowork, &work[i]);
		}
		
		for (int i = 0; i < nthreads; ++i) {
			pthread_join (tid[i], 0);
			float* rout = out0;
			float* bout = work[i].buf;
			for (unsigned long j = 0, k = audio_out::STEREO_BUFFER_SIZE; j < k; ++j) *rout++ += *bout++;
		}*/
		
		
	//}
	
	
	
	
	// with fm/am
	if (dn.find_tone_and_volume ()) {
		
		dn.fill ();
		for (int i = 0; i < dn.num_resonators; ++i) {
			float* rout = out0;
			dn.players[i] (rout, fpb, dn.ams, dn.fms);
		}
				
	}
		
  if (dn.gatl.enabled) {
		float* lout = out0;
		dn.gatl (lout, fpb);
	}

  if (dn.gatr.enabled) {
		float* rout = out0 + 1;
		dn.gatr (rout, fpb);
	}
  
	if (dn.dinfo.delay) {
		float* outl = out0; left_delay (outl, fpb);
		float* outr = out0 + 1; right_delay (outr, fpb);
	}
	
	for (int i = 0, j = dn.num_drones; i < j; ++i) {
		float* dout = out0;
		dn.drones[i].player (dout, fpb);
	}
	
	if (dn.compress) {
		float* outl = out0;
		float* outr = out0 + 1;
		dn.coml.apply (outl, fpb);
		dn.comr.apply (outr, fpb);
	}
    
  if (dn.osc.paused == false) dn.osc.add_samples (out0, fpb);
  
	++clk;
	
	return 0;
		
}

void calc_volume_vars (int h) {
	NUM_VOLUMES = h;
	DELTA_VOLUME = 1./NUM_VOLUMES;
	LAST_VOLUME = NUM_VOLUMES - 1;
}

bool load_globals::load_intervals (const string& fname) {
	
	extern string dotdin;
	ifstream file ((dotdin + fname).c_str(), ios::in);
	if (file) {
		
		file >> ignore >> NUM_INTERVALS;
		for (int i = 0, last = NUM_INTERVALS - 1, pos = 0; i < NUM_INTERVALS; ++i) {

			char note_name;
			float value;

			file >> note_name >> value;
											
			if (i == 0) FIRST_INTERVAL_NAME = note_name; 
			else if (i == last) LAST_INTERVAL_NAME = note_name;
			
			INTERVALS [note_name] = value;
			NOTE_POS [note_name] = pos++;

		}
		
		return true;
		

	} else return false;

}

load_globals::load_globals () {
	extern string dotdin;
	ifstream file ((dotdin + "globals").c_str(), ios::in);
	if (file) {
		cout << "loading globals..." << endl;
		file >> ignore >> APP_NAME;
		file >> ignore >> SCREEN_WIDTH >> SCREEN_HEIGHT >> SCREEN_DEPTH;
		file >> ignore >> SAMPLE_RATE;
		file >> ignore >> INTERVALS_FILE;
		load_intervals (INTERVALS_FILE);
		file >> ignore >> RAGA;
		file >> ignore >> LEFT >> BOTTOM;
		file >> ignore >> HEIGHT;
		TOP = BOTTOM + HEIGHT;
		calc_volume_vars (min (HEIGHT, MAX_HEIGHT));
		file >> ignore >> NUM_MICROTONES;
		cout << "globals file loaded." << endl;
	} else cout << "bad globals file: " << endl;
}

