#ifndef __PLAY
#define __PLAY

#include <note.h>
#include <solver.h>
#include <audio.h>

//
// plays a tone
//

struct play {
	
	// waveform solver
	float x; // current x on the waveform; x goes from 0 to 1. 0 = waveform start, 1 = waveform end ie 1 cycle.
	float dx; // change of x; determines frequency of tone (see note.h)
	float wav [audio_out::CHANNEL_BUFFER_SIZE]; // get y for each x of audio buffer.
	solver* get_wave; // solver linked to multi_curve 
	
	float left, right; // stereo volume of tone
		
  play (solver* s);
  play ();
  
  inline void set_step (float xd) {
    dx = xd;
  }

  inline void set (float xd, float l, float r) {
    dx = xd;
		left = l;
		right = r;
  }
  
  inline void set_wave (solver* w) {
    get_wave = w;
  }
      
  inline void set_volume (float l, float r) {
    left = l;
    right = r;
  }
  
  void operator() (float* out, int n); // output audio
  void operator() (float* out, int n, float* am, float* fm); // output audio with modulation

	private:
	
	  void init ();
    
		
};

#endif
