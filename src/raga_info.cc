#include <ragalist.h>
#include <raga_info.h>
#include <fstream>
#include <iostream>
#include <console.h>
using namespace std;

void raga_info::load_last () {
	
	extern string dotdin;
	ifstream file ((dotdin + "last").c_str(), ios::in);
	
	string n = "bhairav";
	float s = 261.62565;
	
	if (file) {
		string ignore;
		file >> ignore >> s;
		file >> ignore >> n;
	} 
	
	set_sa (s);
	set_raga (n);
}

void raga_info::set_sa (float s) {
	
	sa = s;
	losa = sa / 2.;
	hisa = sa * 2.;
		
}

bool raga_info::set_raga (const string& n) {
	
	extern ragalist ragalst;
	raga r; if (ragalst.get (n, r) == false) {
		cons << console::red << "could not load raga: " << n << eol;
		return false;
	}
	
	name = r.name;
	num_ranges = r.num_notes - 1;
	last_range = num_ranges - 1;
	
	stringstream ss (r.notes);
	for (int i = 0; i < r.num_notes; ++i) {
		string n; ss >> n; notes[i] = n[0];
	}

	if (load_intervals() == false) {
		extern map<char, float> INTERVALS;
		intervals = INTERVALS;
	}
	
	cons << "raga is " << console::green << name << eol;
		
	return true;
	
}


void raga_info::save_last () {
	const char* fname = "last";
	extern string dotdin;
	ofstream file ((dotdin + fname).c_str(), ios::out);
	if (file) {
		file << "sa " << sa << endl;
		file << "raga " << name << endl;
	}
}



bool raga_info::load_intervals () {
	
	extern string dotdin;
	string filename = dotdin + name + ".intervals";

	ifstream file (filename.c_str(), ios::in);
	if (!file) {
		cout << "couldnt load custom tuning from: " << filename << endl; 
		return false;
	}
	
	string ignore;
	
	int n; 
	file >> ignore >> n;
	
	for (int i = 0; i < n; ++i) {
		char c;
		float f;
		file >> c >> f;
		intervals [c] = f;
	}
	
	return true;
	
	
}

void raga_info::save_intervals () {
	extern string dotdin;
	string filename = dotdin + name + ".intervals";
	ofstream file (filename.c_str(), ios::out);
	if (!file) return;
	
	file << "num_intervals " << intervals.size () << endl;
	for (map<char, float>::iterator i = intervals.begin(), j = intervals.end(); i != j; ++i) {
		pair<char, float> p = *i;
		file << p.first << ' ' << p.second << endl;
	}
	
}
