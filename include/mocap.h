#ifndef __mocap
#define __mocap

class basic_editor;

struct mocap { // mouse capture
	
	static const int MAX_POINTS = 1024;
	static const int MAX_CAP = 3 * MAX_POINTS;
	float xy [MAX_CAP];
	int npts;
	int n_3;
	
	float lx, ly;
	
	int repi, rep;
	
  int cur;
  
  int state;
  enum {empty, capturing, finished};
  
  mocap ();
  bool get (float& x, float& y);
  bool add (float wx, float wy); 
  void finish (basic_editor* b);
  void clear ();
  int operator() (); // mocap exists?
    
};

#endif
