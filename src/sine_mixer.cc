#include "sine_mixer.h"
#include "viewwin.h"
#include "input.h"
#include "console.h"
#include "fft.h"
#include <SDL_opengl.h>
#include <iostream>
#include <algorithm>
#include <math.h>
using namespace std;

extern console cons;

sine_mixer::sine_mixer () : sine_levels ("sine_levels") {
	num_harmonics (sine_levels.nlev);
}

sine_mixer::~sine_mixer () {}

void sine_mixer::num_harmonics (int h) {
	nharmonics = h;
	if (nharmonics < 1) nharmonics = 1;
	prep_harmonics ();
}

bool sine_mixer::handle_input () {
	bool result = sine_levels.handle_input ();
	if (keypressed ('t') && sine_levels.hover) {
		get_fft_levels ();
		result = true;
	}
	if (result) {mix (); return true;} 
	return false;
}

bool sine_mixer::get_fft_levels () {
	extern fft fft0;
	levels& fft_levels = fft0.lev;
	for (int i = 0, j = min (sine_levels.nlev, fft_levels.nlev); i < j; ++i) {
		sine_levels.heights[i] = fft_levels.heights[i];
		sine_levels.values[i] = fft_levels.values[i];
		sine_levels.calc ();
	}
	return true;
}

void sine_mixer::draw () {
	sine_levels.draw ();
}

void sine_mixer::prep_harmonics () {
		
	harmonics.clear ();
	for (int i = 0, j = 1; i < nharmonics; ++i, ++j) {
		harmonics.push_back (vector<float>());
		vector<float>& harmonic = harmonics[i];
		harmonic.clear ();
		float dx = 2 * pi * j / (NUM_SINE_SAMPLES - 1);
		float x = 0;
		for (int p = 0; p < NUM_SINE_SAMPLES; ++p) {
			harmonic.push_back (sin(x));
			x += dx;
		}
	}
	
	nonorm.clear ();
	for (int i = 0; i < NUM_SINE_SAMPLES; ++i) nonorm.push_back(0);
	
	mix ();
	
}

void sine_mixer::mix () {
	
	for (int p = 0, q = nonorm.size (); p < q; ++p) nonorm[p] = 0;
	
	for (int i = 0; i < nharmonics; ++i) {
		float lev = sine_levels.values[i];
		vector<float>& harmonic = harmonics[i];
		for (int p = 0, q = nonorm.size (); p < q; ++p) nonorm[p] += (lev * harmonic[p]);
	}
	
	normalise ();
	
}

void sine_mixer::normalise () {
	norm = nonorm;
	float max = *max_element (norm.begin(), norm.end());
	if (max != 0) for (int p = 0, q = norm.size(); p < q; ++p) norm[p] /= max;
}

void sine_mixer::make_curve (multi_curve& c) {
	float dx = 1. / (NUM_SINE_SAMPLES - 1);
	float x = 0;
	c.clear ();
	for (int i = 0, j = norm.size(); i < j; ++i) {
		float y = norm[i];
		c.add_vertex (x, y);
		c.add_left_tangent (x, y);
		c.add_right_tangent (x, y);
		x += dx;
	}
	c.evaluate ();
}
