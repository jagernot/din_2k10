#ifndef __globals
#define __globals

#include <string>
using namespace std;

struct load_globals {
  string ignore;
	bool load_intervals (const string& fname);
	void load_note_colors (const string& fname);
	load_globals ();
};

void calc_volume_vars (int height);

#endif
