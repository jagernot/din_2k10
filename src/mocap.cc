#include <mocap.h>
#include <basic_editor.h>

#include <iostream>
using namespace std;

mocap::mocap () {
  clear ();
}

void mocap::clear () {
	lx = ly = 0;
  cur = -3;
	npts = 0;
  state = empty;
}


bool mocap::get (float& gx, float& gy) {

	if (++repi < rep) {
		gx = xy[cur];
		gy = xy[cur + 1];
		return false;
	} else {
		cur += 3;
		if (cur >= n_3) cur = 0;
		gx = xy[cur];
		gy = xy[cur+1];
		rep = xy[cur+2];
		repi = 0;
		return true;
	}
}

bool mocap::add (float wx, float wy) {
	
	if ((wx == lx) && (wy == ly)) {
		if (npts) ++xy[cur+2]; 
	} else {
		cur += 3;
		if (cur < MAX_CAP) {
			xy[cur] = wx;
			xy[cur+1] = wy;
			xy[cur+2] = 1;
			lx = wx;
			ly = wy;
			++npts;
		} else return false;
	}
	state = capturing;
	return true;
}

void mocap::finish (basic_editor* b) {
			
  if (npts < 2) {
    state = empty;
    return;
  }
	  
	for (int i = 0, j = 0, k = 1; i < npts; ++i, j += 3, k = j + 1) {
		float wx = xy[j], wy = xy[k];
    b->win2obj (wx, wy, xy[j], xy[k]); // convert win space position of mouse into obj space
	}
	
	cur = -3;
	n_3 = 3 * npts - 3;
	repi = 0;
	rep = 0;
	lx = 0;
	ly = 0;
  
  state = finished;
  
}

int mocap::operator() () {
  return (state == finished);
}

