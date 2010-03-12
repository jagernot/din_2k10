#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <console.h>
#include <viewwin.h>
#include <iostream>
using namespace std;

viewport::viewport (const string& name, int w, int h, int depth) {
  
  width = w;
  height = h;
  xmax = width - 1;
  ymax = height - 1;
  xmax_1 = 1.0 / xmax;
  ymax_1 = 1.0 / ymax;
  
  cx = xmax >> 1;
  cy = ymax >> 1;
  
  if (SDL_Init (SDL_INIT_VIDEO) < 0) cout << "SDL init video failed: " << width << ' ' << height << ' ' << SDL_GetError () << endl;
  if (SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1) < 0) cout << "SDL/GL double buffer init failed: " << SDL_GetError () << endl;
  SDL_Surface* s = SDL_SetVideoMode (width, height, depth, SDL_OPENGL | SDL_RESIZABLE);
  if (s) {
    cout << "viewport initialised : " << name << ' ' << width << ' ' << height << ' ' << depth << endl;
    SDL_WM_SetCaption (name.c_str (), name.c_str());
    glViewport (0, 0, width, height);
  }
  else {
    cout << "could not initialise video mode " << width << ' ' << height << ' ' << depth << endl;
    SDL_Quit ();
    throw badviewport();
  }

}

viewport::viewport () {
  cx = cy = 0;
  width=height=0;
  xmax = ymax = 0;
  xmax_1 = ymax_1 = 0;
}

bool viewport::inside (int x, int y) {
	
	bool xi = (x >= 0) && (x <= xmax);
	bool yi = (y >= 0) && (y <= ymax);
	return (xi && yi);
}

void window::calc_panxy () {
  pan_x = pan_scale * width;
  pan_y = pan_scale * height;
}

window::window () : box<float>() {
  width_1 = height_1 = 0;
  aspect_ratio = 0;
}

window::window (float l, float b, float r, float t) {
  set (l, b, r, t);
}

void window::set (float l, float b, float r, float t) {
  operator() (l, b, r, t);
  width_1 = 1.0 / width;
  height_1 = 1.0 / height;
  aspect_ratio = width * 1.0 / height;
  calc_panxy ();
}

void window::get (float& l, float& b, float& r, float& t) {
  l = left;
  b = bottom;
  r = right;
  t = top;
}

void window::panx (int dir) {
  left = left + pan_x * dir;
  right = left + width;
}

void window::pany (int dir) {
  bottom = bottom + pan_y * dir;
  top = bottom + height;
}

void window::zoom (int dir) {

  float dw = zoom_scale * width * dir, dh = dw / aspect_ratio, dw2 = dw / 2, dh2 = dh / 2;
  float l = left - dw2;
  float r = right + dw2;
  float b = bottom - dh2;
  float t = top + dh2;
  float w = r - l, h = t - b;
  if ((w > 0) && (h > 0)) set (l, b, r, t);

}
