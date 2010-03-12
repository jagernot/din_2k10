#ifndef __fft
#define __fft

#include <fftw3.h>
#include <levels.h>
#include <vector>
using namespace std;

struct multi_curve;

struct fft {

	//
	// FFT using FFTW
	//
	
	
	fftwf_plan plan;
	float* in;
	fftwf_complex* out;
	
	vector<float> harms; // harmonics
	levels lev; // harmonics levels
	void draw (); // draw levels

	void go (multi_curve* crv); // do fft on a multiple segment bezier curve
	
	float f; // audio frequency used for fft
	int n; // num samples
	int halfn;
	
	float x, dx; // on bezier curve, or theta/dtheta on sine.

	fft ();
	~fft ();
	
};

#endif
