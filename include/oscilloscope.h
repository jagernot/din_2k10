#ifndef __oscilloscope
#define __oscilloscope

struct oscilloscope {


  static const int MAX_SAMPLES = 512;

	struct sample_t {
		float left, right; // stereo pair
		static float lmin, lmax;
		static float rmin, rmax;
		sample_t () {
			left = right = 0;
		}
	} samples [MAX_SAMPLES];

	int paused;
	int visible;
	
	int viewr;
	int addr;
	

	static const int MAX_HEIGHT = 200;

	oscilloscope ();
	void toggle_pause ();
	void toggle_visible ();
	void add_samples (float* out, int fpb);
	void draw ();

};
#endif
