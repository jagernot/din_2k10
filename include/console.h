#ifndef _CONSOLE
#define _CONSOLE

#define eol console::EOL
#define cll console::CLL
#define tab "  "

#include "box.h"
#include "ui.h"

#include <SDL/SDL_opengl.h>
#include <vector>
#include <string>
#include <sstream>

struct decoration {
  float r, g, b; 
  int lw;
  decoration (float rr = 0.8, float gg = 0.8, float bb = 0.8, int w = 1);
};

struct mesg {
  string text;
  decoration dec;
  mesg (string t, decoration d) : text(t), dec (d) {}
  mesg () {}
};

struct console : ui {

	static const char EOL = '\n'; // end of last mesg
	static const char CLL = '\b'; // clear last mesg
	static const int run = 1; // run command

	static const decoration light;
	static const decoration yellow;
	static const decoration green;
	static const decoration red;
	static const decoration cyan;
	static const decoration bold;

  box<int> win;

  vector<mesg> lines;

  int nlines;
  int lines_per_screen;
  int startl;

  mesg last_line;
  char charbuf [256];

	vector<string> history;
	int hid;

	console ();
	console& operator<< (unsigned int i);
	console& operator<< (unsigned long i); //#~rzr: needed for 64 bits
	console& operator<< (unsigned long long i);
	console& operator<< (int i);
	console& operator<< (const string& s);
	console& operator<< (float f);
	console& operator<< (double d);
	console& operator<< (char c);
	console& operator<< (const decoration& d);

  void push_back (const mesg& ln);
  void up (int i);
  void down (int i);
  void last (); // ensures last mesg is always displayed
  void add_last_line ();
	
	void del ();
	void clear ();
	void pgdn ();
	void pgup ();
	void home ();
	void end ();
	
	void set_window (const box<int>& w);
	void calc_lines_per_screen ();
	void draw ();
	
	bool handle_input ();
	
	int command_mode;
	string command;
	
	bool rollup;
	
	string ps1;
		
	~console();
    
};

template <class T> inline console& operator<< (console& c, const box<T>& b) {
	c << b.left << ' ' << b.bottom << ' ' << b.right << ' ' << b.top;
	return c;
}

extern console cons;
#endif
