#ifndef __crvpt
#define __crvpt

struct crvpt { // curve point
	
	float x, y; 
	float t; // bezier param
	
	// used by solver (see solver.h/solver.cc)
	float m; // slope of seg joining next point
	int inf; // is slope infinity?
	
	crvpt () {
		x = y = t = m = 0;
		inf = 0;
	}
	
	crvpt (float xx, float yy, float tt) {
		x = xx; y = yy; t = tt;
		m = 0; inf = 0;
	}
	
};

#endif
