#ifndef __RANGE
#define __RANGE

#include <box.h>
#include <note.h>
#include <string>
#include <map>
#include <color.h>
using namespace std;

struct range {
  
  //
  // a range of tones from note0 to note1 (both inclusive).
  // the tones between the notes are the microtones.
  //
  
	box<int> extents; // visual of range
	
	note notes[2]; // note0, note1 - these are the end notes
	char intervals[2]; // interval names of the end notes
	
	// display
	string labels[2]; // labels of the end notes
    
  float delta_octave_position; // difference of octave position of the end notes
	float delta_step; // difference of step values of the end notes
  
	
	enum {NONE = -1, LEFT, RIGHT, BOTH};
	void draw_labels (int label_what = LEFT);
	
	void calc_notes (int octave, float start, std::map<char, float>& INTERVALS);
	void set_extents (int left, int bottom, int right, int top);
	
};

#endif
