#ifndef __SOLVER
#define __SOLVER

#include <crvpt.h>

struct multi_curve;

struct solver {

  //
  // given x return y
  //
  // assumes x such that
  // x0 <= x1 <= x2 <= ...... <= xlast
  //
  
	// solved curve
  multi_curve* crv;
  crvpt* pts;
  int npts; // num points in curve
  int last, last_1; // last and last but 1 points of curve
	  
  // line segment on curve expected to contain x
  int start, end;
	float startx, starty;
	float endx, endy;
	float m, inf;
	float ycomp;

  // first and last points
  float firstx, firsty;
  float lastx, lasty;
	
	float result; // last result
	  
  inline int inseg (float x, int l, int r) {
    
    // is x in segment l,r ?
    
    if ( l > -1 && r < npts ) {
      if ( (x >= pts[l].x) && (x <= pts[r].x) ) {
				setseg (l, r);
        return 1;
      }
    }
    return 0;

  }
	
	public:
	
	solver (); 
	solver (multi_curve* crv);
	
	void operator() (multi_curve* crv);
	float operator() (float x); // given x, solve for y
	void operator() (float& x, float& dx, int n, float* res, int dn = 1); // fast solver when x is incremented by dx n times, solution stored in res
	void operator() (float& x, float& dx, int n, float* fm, float* res, int dn = 1); // same as last solver but x is also modulated by fm.
	void operator() (float* ax, int n, int dx = 2); // given an array of x, store solution in same array.
	
	void init (); // (re)init solver - used when crv has changed
	
	inline void setseg (int l, int r) {
		start = l;
		end = r;
		crvpt& ps = pts[start];
		crvpt& pe = pts[end];
		startx = ps.x;
		starty = ps.y;
		endx = pe.x;
		endy = pe.y;
		m = ps.m;
		inf = ps.inf;
		ycomp = starty - m * startx;
		if (inf == 1) ycomp = endy; else ycomp = starty - m * startx;
	}
  
};

#endif
