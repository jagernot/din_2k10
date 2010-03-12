#ifndef __RAGA_INFO 
#define __RAGA_INFO

#include <string>
#include <map>

using namespace std;

struct raga_info {
	
	string name;

	float sa, losa, hisa;
	int num_octaves;

	map <char, float> intervals;

	static const int MAX_NOTES = 16; 
	char notes [MAX_NOTES];
	int num_ranges;
	int last_range;
		
	
	raga_info () {
		num_octaves = 3;
		num_ranges = 0;
		last_range = -1;
		intervals.clear ();
		load_last ();
	}
	
	~raga_info () {
		save_last ();
	}
	
	void load_last ();
	void save_last ();
	
	void set_sa (float s);
	bool set_raga (const string& n);
	
	bool load_intervals ();
	void save_intervals ();
	
	
};

#endif
