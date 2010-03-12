#ifndef __textboard
#define __textboard

#include <vector>
#include <string>
#include <viewwin.h>
using namespace std;

struct text {
	int x, y;
	string txt;
	float r, g, b;
	text (float xx, float yy, window& win, const string& t, float rr=0.5, float gg=0.5, float bb=0.5) : txt(t), r(rr), g(gg), b(bb) {
		extern viewport view;
		win2view (xx, yy, x, y, win, view);
	}
};

class textboard {
	
	vector<text> texts;
	public:
		void draw ();
		void add (const text& t) { texts.push_back (t);}
		void clear () { texts.clear ();}
};

#endif
