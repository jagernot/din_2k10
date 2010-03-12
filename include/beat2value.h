#ifndef __GATER
#define __GATER

#include "multi_curve.h"
#include "curve_editor.h"
#include "curve_library.h"
#include "solver.h"
#include "listeners.h"
#include "audio.h"
#include <string>
using namespace std;

struct beat2value {
	
	beat2value (const string& fname);
	~beat2value ();

	float result [audio_out::CHANNEL_BUFFER_SIZE];
	
	void operator() (float* out, int n);
	void panner (float* out, int n);
	void operator! ();
	
	void curve_edited ();
	
	void apply_modulation (float* out, int n, float amt);
	
	bool enabled;
	
	string crvname;
	multi_curve crv;
	solver sol;
			
	float bpm;
	void set_bpm (float n);
	float get_bpm ();

	float now;
	float delta;
	
	void load (const string& fname);
	void save (const string& fname);
  
};

#endif
