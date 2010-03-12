#ifndef __sine_mixer
#define __sine_mixer

#include <vector>
#include "box.h"
#include "levels.h"
#include "ui.h"
#include "multi_curve.h"

using namespace std;

struct sine_mixer : ui {

	static const float pi = 3.14159;
	static const int NUM_SINE_SAMPLES = 100;
		
	int nharmonics;
	vector< vector<float> > harmonics;
	
	// output
	vector<float> nonorm, norm; 
	void normalise ();
	
	levels sine_levels;
		
	sine_mixer ();
	~sine_mixer ();
	
	void draw ();
	bool handle_input ();
	
	void prep_harmonics ();
	void num_harmonics (int n);
	void mix ();
	void make_curve (multi_curve& c);
	bool get_fft_levels ();
	
};

#endif
