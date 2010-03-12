#ifndef __levels
#define __levels

#include <string>
#include <vector>
#include "ui.h"
#include "box.h"
using namespace std;

struct levels : ui {

	string name; // name of this control
	
	void load (); // load settings
	void save (); // save settings
	
	// gui vars
	box<int> bbox;
	int elem_width; 
	int height;
	bool hover;
	
	int nlev; // number of levels
	int lastlev;
	
	int lev; // edited level
	float val; // edited val
	int hgt; // edited height
	
	bool moving; // moving this control?

	bool editing; // editing individual elements?
	int editable; // is control editable?
	bool edited; // values edited?
	
	int saveable; // levels saved on quit?
	
	vector<float> values; // 0 to 1 for each level
	vector<int> heights; // 0 to height
	
	levels (const string& s);
	~levels ();
	
	bool handle_input ();
	
	// color
	float r, g, b;
	float a, a0;
	
	void draw ();
	
	void clear ();
	void calc ();
	void set (int i, float v, int h = -1);
	
	void chkpos ();
	
};

#endif
