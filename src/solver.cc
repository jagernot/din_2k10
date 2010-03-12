#include <solver.h>
#include <multi_curve.h>

solver::solver () {
  crv = 0;
}

solver::solver (multi_curve* c) {
  operator() (c);
}

void solver::operator() (multi_curve* c) {
  crv = c;
	init ();
}

void solver::init () {
    
  pts = crv->get_points ();
  npts = crv->num_points ();
    
  // set current segment to first segment
	setseg (0, 1);
  
  // last, last but 1 points
  last = npts - 1;
  last_1 = last - 1;

  // find first and last x and its corresponding y
  // we assume x0 <= x1 <= x2 <= ..... <= xlast
  firstx = pts[0].x; firsty = pts[0].y;
  lastx = pts[last].x; lasty = pts[last].y;
	

}

float solver::operator() (float x) {
  calcy:
	if (x >= startx && x <= endx ) {
		result = ycomp + m * x;
		return result;
  } else {
    // find segment containing x

    // we search segments to the right and segments to
    // the left - potentially faster than searching
    // from first segment to last segment each time

    int endl = start, startl = endl - 1;
    int startr = end, endr = startr + 1;

    while (startl > -1 || endr < npts) {
			if (inseg (x, startr, endr)) goto calcy; else if (inseg (x, startl, endl)) goto calcy;
      ++startr; ++endr;
      --startl; --endl;
    }
				
		if (x < firstx) return firsty; else if (x > lastx) return lasty; else return result;

  }

}

// solves y for x with modulation of x
void solver::operator() (float& x, float& dx, int q, float* mod, float* y, int dp) {
	
	for (int p = 0; p < q; p += dp) {
		x += (mod[p] + dx);
		calcy:
		if ( x >= startx && x <= endx ) {
				result = ycomp + m * x;
				y[p] = result;
		} else if (x > lastx) {
			x -= lastx;
			setseg (0, 1);
			goto calcy;
		} else if (x < firstx) {
			x = firstx;
			setseg (0, 1);
			goto calcy;
		} else {

			// find segment containing x

			// we search segments to the right and segments to
			// the left - potentially faster than searching
			// from first segment to last segment each time

			int endl = start, startl = endl - 1;
			int startr = end, endr = startr + 1;

			while (startl > -1 || endr < npts) {
				if (inseg (x, startr, endr)) goto calcy; 
				else if (inseg (x, startl, endl)) goto calcy;
				++startr; ++endr;
				--startl; --endl;
			}

			if (x < firstx) y[p] = firsty; else if (x > lastx) y[p] = lasty; else y[p] = result;
		}
	}
}

// solves y for x
void solver::operator() (float& x, float& dx, int q, float* y, int dp) {
	for (int p = 0; p < q; p += dp) {
		x += dx;
		calcy:
		if ( x >= startx && x <= endx ) {
				result = ycomp + m * x;
				y[p] = result;
		} else if (x > lastx) {
			x -= lastx;
			setseg (0, 1);
			goto calcy;
		} else if (x < firstx) {
			x = firstx;
			setseg (0, 1);
			goto calcy;
		} else {
			// find segment containing x

			// we search segments to the right and segments to
			// the left - potentially faster than searching
			// from first segment to last segment each time

			int endl = start, startl = endl - 1;
			int startr = end, endr = startr + 1;

			while (startl > -1 || endr < npts) {
				if (inseg (x, startr, endr)) goto calcy; 
				else if (inseg (x, startl, endl)) goto calcy;
				++startr; ++endr;
				--startl; --endl;
			}
			if (x < firstx) y[p] = firsty; else if (x > lastx) y[p] = lasty; else y[p] = result;
		}
	}
}

// solves array of x. stores solution ie y in the same array at corresponding location
void solver::operator() (float* ax, int q, int dx) {
	
	float x;
	for (int p = 0; p < q; ++p, ax += dx) {
		x = *ax;
		calcy:
		if ( x >= startx && x <= endx ) {
				result = ycomp + m * x;
				*ax = result;
		} else if (x > lastx) {
			x -= lastx;
			setseg (0, 1);
			goto calcy;
		} else if (x < firstx) {
			x = firstx;
			setseg (0, 1);
			goto calcy;
		} else {
			// find segment containing x

			// we search segments to the right and segments to
			// the left - potentially faster than searching
			// from first segment to last segment each time

			int endl = start, startl = endl - 1;
			int startr = end, endr = startr + 1;

			while (startl > -1 || endr < npts) {
				if (inseg (x, startr, endr)) goto calcy; 
				else if (inseg (x, startl, endl)) goto calcy;
				++startr; ++endr;
				--startl; --endl;
			}
			if (x < firstx) *ax = firsty; else if (x > lastx) *ax = lasty; else *ax = result;
		}
	}

}
