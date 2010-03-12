#ifndef __delay
#define __delay

class delay { // 1 channel delay
  
  int nsamples;
  float* samples;
  int id;

	float msecs;
  float feedback;
	float volume;
	    
  public:
    delay (float msecs, float str, float mas);  // time, strength & master volume
		delay ();
    void operator() (float* out, int nz);
    ~delay ();
		void set (float t, float s, const float m);
		void get (float& t, float& s, float& m);

};
#endif
