#ifndef __RESONATOR
#define __RESONATOR

#include <multi_curve.h>
#include <solver.h>

struct resonator {
  
  static const int MAX_RESONATORS = 20;
  
  multi_curve strength; // resonator strength across the entire tone range
  solver get_strength; // solver for resonator strength

  multi_curve left, right; // stereo response of resonator to driver input 

  multi_curve wave; // resonator waveform
  solver get_wave; // solver for resonator waveform
  
  enum {STRENGTH=0xf000, WAVE=0x0f00, LEFT=0x00f0, RIGHT=0x000f}; // possible edits
  unsigned int edited; // what was edited? strength/response/wave

  bool disabled; // is disabled?
  
  resonator () {
    edited = 0;
    disabled = false;
    get_strength (&strength);
    get_wave (&wave);
  }
  
  void update_solvers () {
		get_strength.init ();
		get_wave.init ();
  }

};

#endif
