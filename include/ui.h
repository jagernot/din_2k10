#ifndef __ui
#define __ui

#include <vector>
using namespace std;

struct ui {
  virtual void enter () {}
  virtual void leave () {}
  virtual bool handle_input() {return true;}
  virtual void draw() {}
  virtual void bg () {}
  virtual ~ui () {}
};

struct curve_editor;

struct uis_t : ui {
  
	ui* current;
  ui* last;
	curve_editor* current_curve_editor;
  vector<ui*> uis;

	uis_t ();
  void set_current (ui* u, int crved = 1);
  bool handle_input ();
	void draw ();
	
};


#endif
