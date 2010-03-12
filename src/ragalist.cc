#include <ragalist.h>
#include <console.h>
#include <algorithm>
#include <fstream>
#include <iostream>
using namespace std;

extern console cons;

ragalist::ragalist () {
	load ();
}

ragalist::~ragalist () {
	save ();
}

void ragalist::add (stringstream& ss) {
	string nam; ss >> nam;
	if (nam != "") {
		raga r;
		r.name = nam;
		string notes;
		while (1) {
			string s; ss >> s;
			if (s == "") break; else {
				notes = notes + s + " ";
				++r.num_notes;
			}
		}
		r.notes = notes;
		ragas.push_back (r);
		cons << console::green << "added raga: " << nam << eol;
	}
}

void ragalist::ls () {
	for (int i = 0, j = ragas.size(); i < j; ++i) {
		raga& r = ragas[i];
		cons << console::yellow << ' ' << r.name << ' ' << r.notes << eol;
	}
}

void ragalist::remove (const string& nam) {
	raga r; r.name = nam;
	vector<raga>::iterator i = find (ragas.begin(), ragas.end(), r);
	if (i != ragas.end()) {
		ragas.erase (i); 
		cons << console::green << "removed raga: " << nam << eol;
	} else cons << console::red << "raga: " << nam << " not found." << eol;
}

void ragalist::load () {
	extern string dotdin;
	ifstream file ((dotdin + "ragalist").c_str(), ios::in);
	if (file) {
		int j;
		string ignore; file >> ignore >> j;
		for (int i = 0; i < j; ++i) {
			raga r;
			string nam; file >> nam; r.name = nam;
			file >> r.num_notes;
			for (int k = 0; k < r.num_notes; ++k) {
				string s; file >> s;
				r.notes = r.notes + s + ' ';
			}
			ragas.push_back (r);
		}
	} else cout << "bad ragalist file" << endl;
}

void ragalist::save () {
	extern string dotdin;
	ofstream file ((dotdin + "ragalist").c_str(), ios::out);
	if (file) {
		int j = ragas.size();
		file << "num_ragas " << j << eol;
		for (int i = 0; i < j; ++i) {
			raga& r = ragas[i];
			file << r.name << " " << r.num_notes << ' ' << r.notes << eol;
		}
	}
}

bool ragalist::get (const string& n, raga& r) {
	r.name = n;
	vector<raga>::iterator i = find (ragas.begin(), ragas.end(), r);
	if (i != ragas.end()) {
		r = *i;
		return true;
	}
	return false;
}
