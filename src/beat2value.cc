#include "beat2value.h"
#include "console.h"
#include <fstream>
#include <iostream>
using namespace std;

extern console cons;

beat2value::beat2value (const string& c) : crvname (c), crv (c)  {
	
  now = 0;
	bpm = 120;
  enabled = false;
  set_bpm (bpm);
  sol(&crv);
  curve_edited ();
}

void beat2value::operator! () {
	if (enabled) {
		enabled = false;
	} else {
		enabled = true;
	}
}

beat2value::~beat2value () {
  crv.save (crvname);
}

void beat2value::operator() (float* out, int n) {
	sol (now, delta, n, result);
	for (int i = 0; i < n; ++i, out += 2) *out *= result[i];
}

void beat2value::panner (float* out, int n) {
	sol (now, delta, n, result);
	for (int i = 0; i < n; ++i, out += 2) {
		float b = result[i];
		*out *= b;
		*(out + 1) *= (1 - b);
	}
}

void beat2value::apply_modulation (float* out, int n, float depth) {
	sol (now, delta, n, result);
	for (int i = 0; i < n; ++i) *out++ = (depth * result[i]);
}

void beat2value::curve_edited () {
	sol.init ();
}

void beat2value::set_bpm (float n) {
  if (n < 0) n = -n; 
  bpm = n;
  float bps = bpm / 60.0;
  extern int SAMPLE_RATE;
  delta = bps * 1. / SAMPLE_RATE;
}

float beat2value::get_bpm () { return bpm; }
