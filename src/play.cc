#include <play.h>
#include <iostream>
using namespace std;

play::play (solver *w) {
  init ();
  set_wave (w);
}

void play::init () {
  get_wave = 0;
  x = 1;
  dx = 0;
  left = right = 0;
}

play::play () {
  init ();
}

void play::operator() (float* out, int n) {
  // no modulation audio output
	
	/*if ((left == 0) && (right == 0)) {
		x += (n * dx);
		if (x >= 1) {
			int ix = (int) x;
			x = x - ix;
		}
		return; 
	}*/
	
	(*get_wave)(x, dx, n, wav);
  for (int i = 0; i < n; ++i) {
    *out++ += (left * wav[i]);
    *out++ += (right * wav[i]);
  }
}

void play::operator() (float* out, int n, float* am, float* fm) {
  // AM and FM modulation in audio output
	(*get_wave)(x, dx, n, fm, wav);
  for (int i = 0; i < n; ++i) {
    *out++ += ((left + am[i]) * wav[i]);
    *out++ += ((right + am[i]) * wav[i]);
  }
}
