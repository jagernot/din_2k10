#ifndef __glyph
#define __glyph

#include <vector>
#include <line.h>
using namespace std;

struct glyph {
  vector<line> lines;
  float width, height;
  
  glyph (const vector<line>& vl);
  glyph (int w = 3, int h = 8) {
		width = w;
		height = h;
  }
  void find_width_height ();
};

#endif
