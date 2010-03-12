#include <note.h>

note::note () {

  octave_position = 0;
  frequency = 0;
  step = 0;
  
}

void note::set_frequency (float f) {
  
  frequency = f;
  hz2step (f, step);
  
}

float note::get_frequency () {
  return frequency;
}

void hz2step (float& hz, float& step) {
  extern int SAMPLE_RATE;
  step = hz * 1.0 / SAMPLE_RATE;
  
  
}


