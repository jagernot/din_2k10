#ifndef __chrono
#define __chrono

struct chrono {
	
	double tk; // tick now
	int tk_sec; // ticks per sec
	int tk_callb; // ticks per audio callback
	
	double t; // time now
	double dt; // time per audio callback
	
	chrono (int tks);
	chrono& operator++ ();
	int sec2tk (float s);
		
};


struct timer {
	
	int start;
	
	int duration;
	
	// how many triggers in how many seconds
	int ntrigs;
	float nsecs; 
	
	enum {disabled, running};
	int state;
	
	timer (int nt = 0, float ns = -1); 
	void calc_duration ();
	void go ();
	int eval ();
		
};

#endif
