#ifndef __NOTE
#define __NOTE

void hz2step (float& hz, float& step);

struct note {
  
  //
  // a musical note - also a tone
  //
    
  float frequency;
  float step; // amount to step in x along the waveform every sample to get the audio output at that sample.
	float octave_position; // octave number + note position in octave
	
	note ();
	float get_frequency ();
	void set_frequency (float f);

};



#endif
