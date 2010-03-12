#include <delay.h>
#include <string.h>
#include <stdlib.h>

delay::delay (float ms, float fbk, float mas) : nsamples(0), samples(0) {
	set (ms, fbk, mas);
}

delay::delay () : nsamples(0), samples(0) {
	set (0, 0, 0);
}

void delay::set (float ms, float fbk, float mas) {
	
	if (ms != msecs) {
		msecs = ms;
		extern int SAMPLE_RATE;
		int n = (int)(msecs * SAMPLE_RATE / 1000.0 + 0.5);
		if (n == 0) n = 1;
		if (n > nsamples) {
			float* p = samples;
			float* q = (float *) calloc (n, sizeof (float)); // init to 0
			samples = q;
			if (p) free (p);
		}
		nsamples = n;
		id = 0;
	}
	
	feedback = fbk;
	volume = mas;
	
}

void delay::get (float& t, float& f, float& m) {
	t = msecs;
	f = feedback;
	m = volume;
}

void delay::operator() (float* out, int n) {
  
	float dl, f;
  for (int i = 0; i < n; ++i) {
    dl = samples [id];
		f = *out + feedback * dl;
		*out += volume * f;
    samples [id] = f;
    out += 2; // 1 channel delay so skip interleaved channel.
    if (++id == nsamples) id = 0;
    
  }
}

delay::~delay() {
  if (samples) free (samples);
}

