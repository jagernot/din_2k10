#ifndef __input
#define __input

#include <tick.h>
#include <SDL/SDL.h>

struct keyboard {
	
		int nkeys; // number of keys
		Uint8* now; // key state now (1 - down, 0 - up)
    Uint8* last; // last state of keys
    unsigned int* querky; // list of keys whose state was queried
    int nquer; // num of queried keys
    clock_t* tstamp; // time stamp when key was first down
    clock_t* repeat; // time for key to repeat

		keyboard () {

			SDL_GetKeyState (&nkeys);

      last = new Uint8 [nkeys];

      querky = new unsigned int [nkeys];
      nquer = 0;

      tstamp = new clock_t [nkeys];
      repeat = new clock_t [nkeys];

		}
		
		void read () { // current keyboard state
			now = SDL_GetKeyState (0);
		}
		
		void save () {
		  for (int i = 0; i < nquer; ++i) {
		    unsigned int k = querky[i];
		    last[k] = now[k];
      }
      nquer = 0;
    }
		
		~keyboard () {
      delete[] last;
		  delete[] querky;
      delete[] tstamp;
      delete[] repeat;
    }

};

extern keyboard keybd;

inline int nkeys () {
	return keybd.nkeys;
}

inline int keydown (unsigned int k) {
  keybd.querky [keybd.nquer++] = k;
  return keybd.now[k];
}

inline int keyup (unsigned int k) {
  int wkd = keybd.last[k];
  int kn = keybd.now[k];
  return (!kn && wkd);
}

inline int waskeydown (unsigned int k) {
	return keybd.last[k];
}

inline int keypressed (unsigned int k) {
	int wkd = waskeydown (k), kd = keydown (k);
	if (kd && !wkd) return 1; else return 0;
}


inline int keypressedd (unsigned int k, clock_t first_t=secs2ticks(1./4), clock_t const_t=secs2ticks(1./32)) {

	/* keypressed with key repeat */

	int wkd = waskeydown(k), kd = keydown(k);
	if (kd) {
		if (!wkd) {
			keybd.tstamp[k] = clock();
			keybd.repeat[k] = first_t;
			return 1;
		} else {
			clock_t t = clock();
			clock_t dt = t - keybd.tstamp[k];
			if (dt > keybd.repeat[k]) {
				keybd.tstamp[k] = t;
				keybd.repeat[k] = const_t;
				return 1;
			}
		}
	}
	return 0;
}

#endif
