#ifndef __compressor
#define __compressor

#include "curve_editor.h"
#include "multi_curve.h"
#include "solver.h"

struct compressor;

struct compressor_listener : curve_listener {
	compressor& c;
	compressor_listener (compressor& cc) : c (cc) {}
	void edited (int i);
};


struct compressor { // dynamic range compression using a bezier curve based transfer function
	
	string fname;
	multi_curve crv;
	solver apply;
	compressor_listener lis;
	
	compressor (const string& f);
	~compressor ();
	
}

;

#endif
