#include "fft.h"
#include "multi_curve.h"
#include "solver.h"
#include "math.h"
#include <algorithm>
using namespace std;

fft::fft () : lev ("fft_levels") {
	
	n = 100; // 2 x number of harmonics
	halfn = n / 2;
	dx = 1. / n;
	x = 0;
	
	// use fftw library for fft
	in = (float *) fftwf_malloc (sizeof(float) * n);
	out = (fftwf_complex *) fftwf_malloc (sizeof (fftwf_complex) * (halfn + 1));
	
	extern string dotdin;
	string fname = dotdin + "fftwf_wisdom";
	cout << "measuring FFTW. please wait..." << endl;
	FILE* wis = fopen (fname.c_str(), "r");
	if (wis) {
		cout << "found fftwf_wisdom. this will be quick" << endl;
		fftwf_import_wisdom_from_file (wis);
		fclose (wis);
	}	else {
		cout << "fftwf_wisdom not found" << endl;
	}
	
	plan = fftwf_plan_dft_r2c_1d (n, in, out, FFTW_MEASURE);

	multi_curve crv ("default_wave.txt");
	solver s (&crv);
	s (x, dx, n, in);

	fftwf_execute (plan);
	
	cout << "measurement finished." << endl;
	
}

fft::~fft () {
		extern string dotdin;
		string fname = dotdin + "fftwf_wisdom";
		FILE* wis = fopen (fname.c_str(), "w");
		if (wis) {
			cout << "exporing fftwf_wisdom for quick measurement next time." << endl;
			fftwf_export_wisdom_to_file (wis);
			cout << "exported fftwf_wisdom" << endl;
			fclose (wis);
		}
	fftwf_destroy_plan (plan);
	fftwf_free (in);
	fftwf_free (out);
}

void fft::go (multi_curve* crv) {
	
	// do fft on a bezier multi curve
	
	solver s (crv);
	
	x = 0; 
	s (x, dx, n, in);
	
	fftwf_execute (plan);
	
	harms.clear ();
	for (int i = 1, j = halfn +1; i < j; ++i) {
		float x = out[i][0];
		float y = out[i][1]; 
		float x2 = x*x;
		float y2 = y*y;
		harms.push_back (sqrt (x2 + y2));
		// ignore phase (atan2 (y2/x2)) for now, but have to add in later revision.
	}
	
	float m0 = *min_element (harms.begin(), harms.end());
	float m1 = *max_element (harms.begin(), harms.end());
	float mm = m1 - m0;
	if (m1 == m0) return; 
	
	for (int i = 0, j = harms.size(); i < j; ++i) {
		float dm = harms[i] - m0;
		float y = dm / mm;
		lev.set (i, y);
	}

}
