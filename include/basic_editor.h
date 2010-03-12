#ifndef __basic_editor
#define __basic_editor

#include "viewwin.h"
#include "point.h"
#include "help.h"
#include "mocap.h"
#include <map>
using namespace std;

struct basic_editor {
  
	window win; // edit window
  
	// mouse pos in window
	float win_mousex, win_mousey; 
  float prev_win_mousex, prev_win_mousey;
  
  // object space <> window space mapping
	point<int> win_chunk;
	point<float> obj_chunk;
	point<float> win_per_obj;
	point<float> obj_per_win;
	int win_resolution;
	float obj_resolution;

	// editor features
	//

	// snapping support
  string snap_what;

	static const int buf_size = 256;
	char buf[buf_size];
	
  help helptext;
	
	mocap mocap0;
	
	int pan, zoom; // pan & zoom deltas to control pan and zoom speed
	
	basic_editor (const string& settingsf = "../data/basic_editor.txt", const string& helpf = "../data/basic_editor_help.txt");
	virtual ~basic_editor ();
	
	inline void calc_win_mouse () {
		extern viewport view;
		extern int mousex, mousey;
		view2win (mousex, mousey, win_mousex, win_mousey, view, win);
	}

	inline void save_win_mouse () {
		prev_win_mousex = win_mousex;
		prev_win_mousey = win_mousey;
	}
	
	void obj2win (const point<float>& p, int& wx, int& wy);
	void obj2win (const float& ox, const float& oy, int& wx, int& wy);
	void win2obj (const float& dx, const float& dy, float& cx, float& cy);
	
	inline void get_win_mouse (float& x, float& y) {
		x = win_mousex;
		y = win_mousey;
	}
	
	inline const point<float>& get_obj_chunk () {
		return obj_chunk;
	}
	
	inline float get_obj_resolution () {
		return obj_resolution;
	}
	
	void load (const string& fname);
	void load (ifstream& file);
	void save (ofstream& file);

	void set_win_chunk (int x, int y);
	bool handle_input ();
	void snap (float& x, float& y);
	bool is_snapx ();
	bool is_snapy ();
	
	void project ();
	void unproject ();
	void draw ();
	void draw_snaps ();



};
#endif
