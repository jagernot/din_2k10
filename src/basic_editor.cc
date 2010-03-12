#include <SDL/SDL_opengl.h>
#include <fstream>
#include <iostream>

#include "basic_editor.h"
#include "font.h"
#include "input.h"
#include "console.h"

extern console cons;

basic_editor::basic_editor (const string& settingsf, const string& helpf) : helptext (helpf) {
  load (settingsf);
  pan = zoom = 1;
}

basic_editor::~basic_editor () {}

void basic_editor::obj2win (const point<float>& v, int& wx, int& wy) {
  wx = (int) (win_per_obj.x * v.x);
  wy = (int) (win_per_obj.y * v.y);
}

void basic_editor::obj2win (const float& ox, const float& oy, int& wx, int& wy) {
  wx = (int) (win_per_obj.x * ox);
  wy = (int) (win_per_obj.y * oy);
}

void basic_editor::win2obj (const float& wx, const float& wy, float& ox, float& oy) {
  ox = obj_per_win.x * wx;
  oy = obj_per_win.y * wy;
}

void basic_editor::load (const string& fname) {
  
	extern string dotdin;
  ifstream file ((dotdin + fname).c_str (), ios::in);
  if (!file) return;
  load (file);
  
}

void basic_editor::load  (ifstream& file) {

  string ignore;
  float l, b, r,  t;
  file >> ignore >> l >> b >> r >> t;
  win.set (l, b, r, t);

  file >> ignore >> win_chunk.x >> win_chunk.y;
  file >> ignore >> obj_chunk.x >> obj_chunk.y;
  
  win_per_obj (win_chunk.x / obj_chunk.x, win_chunk.y / obj_chunk.y);
  obj_per_win (obj_chunk.x / win_chunk.x , obj_chunk.y / win_chunk.y);

  file >> ignore >> snap_what;

  file >> ignore >> win_resolution;
  int dummy1 = 0; float dummy2 = 0;
  win2obj (win_resolution, dummy1, obj_resolution, dummy2);
  
  file >> ignore >> pan;
  file >> ignore >> zoom;
  
}

void basic_editor::save (ofstream& file) {

  file << "window " << win.left << ' ' << win.bottom << ' ' << win.right << ' ' << win.top << endl;
  file << "win_chunk " << win_chunk.x << ' ' << win_chunk.y << endl;
  file << "obj_chunk " << obj_chunk.x << ' ' << obj_chunk.y << endl;
  file << "snap " << snap_what << endl;
  file << "win_resolution " << win_resolution << endl;
  file << "delta_pan " << pan << endl;
  file << "delta_zoom " << zoom << endl;
}

bool basic_editor::handle_input () {
  
  calc_win_mouse ();
  
  
  // mouse capture
  
	if (mocap0.state == mocap::capturing) mocap0.add (win_mousex, win_mousey);
	if (keypressed (SDLK_F7)) {
		if (mocap0.state != mocap::capturing) {
			mocap0.clear ();
			mocap0.state = mocap::capturing;
			cons << console::green << "capturing mouse" << eol;
		} else {
			mocap0.finish (this);
			cons << console::red << "mouse captured" << eol;
		}
	}
  
  // movement
	
  if (keydown ('a')) {
    if (keypressedd (SDLK_LSHIFT)) { 
      if (--pan < 1) pan = 1;
      cons << cll << "pan = " << pan;
    } else win.panx (-pan);
  }
  if (keydown ('d')) {
    if (keypressedd (SDLK_LSHIFT)) {
      ++pan;
      cons << cll << "pan = " << pan;
    } else win.panx (+pan);
  }
 
  if (keydown ('w')) win.pany (+pan);
  if (keydown ('s')) win.pany (-pan);
  
  if (keydown ('q')) {
    if (keypressedd (SDLK_LSHIFT)) { 
      if (--zoom < 1) zoom = 1;
      cons << cll << "zoom = " << zoom;
    } else win.zoom (zoom);
  }
  if (keydown ('e')) {
    if (keypressedd (SDLK_LSHIFT)) {
      ++zoom;
      cons << cll << "zoom = " << zoom;
    } else win.zoom (-zoom);
  }
	
  // snap
  if (keypressed ('x')) snap_what = "x";
  else if (keypressed ('y')) snap_what = "y";
  else if (keypressed ('b')) snap_what = "both";
  else if (keypressed ('n')) snap_what = "none";
  
  if (keydown (SDLK_F5)) {
    if (keydown (SDLK_LSHIFT)) set_win_chunk (win_chunk.x, --win_chunk.y); else set_win_chunk (--win_chunk.x, win_chunk.y);
  } else if (keydown (SDLK_F6)) {
    if (keydown (SDLK_LSHIFT)) set_win_chunk (win_chunk.x, ++win_chunk.y); else set_win_chunk (++win_chunk.x, win_chunk.y); 
  }
  
  /*if (keypressed ('p')) {
  	show_cursor = !show_cursor;
  	if (show_cursor) cons << "cursor is on" << eol; else cons << "cursor is off" << eol;
  }*/
  
  
  return true;
}

void basic_editor::set_win_chunk (int x, int y) {
  
  if (x < 1) x = 1;
  if (y < 1) y = 1;
  
  win_chunk.x = x;
  win_chunk.y = y;
  
  float wx = win_chunk.x / obj_chunk.x, wy = win_chunk.y / obj_chunk.y;
  win_per_obj (wx, wy);
  obj_per_win (1./wx, 1./wy);
  
}

bool basic_editor::is_snapx () {
  const string x = "x";
  const string both = "both";
  return (snap_what == x || snap_what == both);
}

bool basic_editor::is_snapy () {
  const string y = "y";
  const string both = "both";
  return (snap_what == y || snap_what == both);
}

void basic_editor::snap (float& x, float& y) {
  x = win_mousex;
  y = win_mousey;
  if (is_snapx()) {
    float s = win_mousex / win_chunk.x;
    if (s < 0) s -= 0.5; else s += 0.5;
    x = (int) s * win_chunk.x;
  }
  if (is_snapy()) {
    float s = win_mousey / win_chunk.y;
    if (s < 0) s -= 0.5; else s += 0.5;
    y = (int)s * win_chunk.y;
  }
}

void basic_editor::project () {
  glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();
      glLoadIdentity ();
      glMatrixMode (GL_PROJECTION);
      glPushMatrix ();
        glLoadIdentity ();
        glOrtho (win.left, win.right, win.bottom, win.top, -1, 1);
}

void basic_editor::unproject () {
      glPopMatrix ();
      glMatrixMode (GL_MODELVIEW);
    glPopMatrix ();

}

void basic_editor::draw  () {
  
  // must always be wrapped by project, unproject
  
  draw_snaps ();

  
  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();
    glLoadIdentity ();
    extern viewport view;
    glOrtho (0, view.xmax, 0, view.ymax, -1, 1);
    float wx, wy; snap (wx, wy);
    float cx, cy; win2obj (wx, wy, cx, cy);
    sprintf (buf, " %.3f, %.3f", cx, cy);
    glColor3f (1,1,1);
    int vx, vy; win2view (wx, wy, vx, vy, win, view);
    draw_string (buf, vx, vy);
  glPopMatrix ();

  glColor3f (0.6, 0.6, 0.6);
  glBegin (GL_LINE_STRIP);
    glVertex2f (win.left, wy);
    glVertex2f (wx, wy);
    glVertex2f (wx, win.bottom);
  glEnd ();

    
}

void basic_editor::draw_snaps () {

  //
  // draw snap lines
  //

  static const float sr = 0.1, sg = sr, sb = sg; // snap color

  //glEnable (GL_LINE_STIPPLE);
  //glLineStipple (1, 0x1381);

  glColor3f (sr, sg, sb);

  if (is_snapx()) { // lines along y

    int l = (int)(win.left / win_chunk.x);
    int startx = l * win_chunk.x;
    int r = (int)(win.right / win_chunk.x);
    int endx = r * win_chunk.x;

    while (startx <= endx) {

      glBegin (GL_LINES);
        glVertex2f (startx, win.bottom);
        glVertex2f (startx, win.top);
      glEnd ();
      startx += win_chunk.x;

    }

  }

  if (is_snapy()) { // lines along x

    int b = (int)(win.bottom / win_chunk.y);
    int starty = b * win_chunk.y;
    int t = (int)(win.top / win_chunk.y);
    int endy = t * win_chunk.y;

    while (starty <= endy) {
      glBegin (GL_LINES);
        glVertex2f (win.left, starty);
        glVertex2f (win.right, starty);
      glEnd ();
      starty += win_chunk.y;
    }

  }

  //glDisable (GL_LINE_STIPPLE);
}
