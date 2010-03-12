#ifndef __phrasor
#define __phrasor

#include <vector>
#include "box.h"
using namespace std;

struct phrase_data {
  int left, bottom, right, top; // window data
  int win_mousex, win_mousey; // mouse pos in window
  int mousex, mousey; // mouse pos in viewport
	
	phrase_data () {
		left = bottom = right = top = 0;
		win_mousex = win_mousey = 0;
		mousex = mousey = 0;
	}

  phrase_data (int l, int b, int r, int t, int wx, int wy, int mx, int my) {
    left = l; bottom = b; right = r; top = t;
    win_mousex = wx; win_mousey = wy;
    mousex = mx; mousey = my;
  }
  
};

struct phrasor {
  
  vector <phrase_data> data;
	int sz, lst;
	
  bool warpmouse;
  
  int state;
  enum {stopped, recording, playing, paused};
  
  int cur;
  int cue;
    
  phrasor () {
    state = stopped;
    warpmouse = true;
    cur = 0;
    cue = 0;
		sz = 0;
		lst = -1;
  }
      
  void get (box<int>& w, int& mousex, int& mousey, int& win_mousex, int& win_mousey) {
    phrase_data& pd = data[cur];
    w.left = pd.left; w.bottom = pd.bottom;
    w.right = pd.right; w.top = pd.top;
    mousex = pd.mousex; mousey = pd.mousey;
    win_mousex = pd.win_mousex; win_mousey = pd.win_mousey;
  }
  
  void get (int& win_mousex, int& win_mousey) {
    phrase_data& pd = data[cur];
    win_mousex = pd.win_mousex; 
    win_mousey = pd.win_mousey;
  }
  
  void next () {
    int c = cur + 1;
    if (c >= sz) cur = 0; else cur = c;
  }
  
  void clear () {
    state = stopped;
    data.clear ();
    cur = cue = 0;
		sz = 0; lst = -1;
  }
  
  void jog (int j) {
    int c = cur + j;
    if (c >= sz) cur = 0; else if (c < 0) cur = lst; else cur = c;
  }
  
  void play () {
    if (cur < sz) state = playing;
  }
  
  void set_cue () {
    cue = cur;
  }
  
  void goto_cue () {
    cur = cue; 
  }
	
	void validate () {
		sz = data.size ();
		lst = sz - 1;
	}
	
	void draw ();
      
};

#endif
