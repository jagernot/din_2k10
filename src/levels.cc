#include "levels.h"
#include "fstream"
#include "viewwin.h"
#include "input.h"
#include "console.h"
#include "utils.h"
#include "font.h"
#include <SDL_opengl.h>
#include <iostream>
using namespace std;

extern console cons;

levels::levels (const string& s) {
	name = s;
	load ();
}

levels::~levels () {
	save ();
}

void levels::load () {
	
	extern string dotdin;
	ifstream file ((dotdin + name).c_str(), ios::in);
	string ignore;
	
	int left, bottom;
	if (!file) {
		cout << "cant read settings for " << name << ", using defaults." << endl;
		nlev = 10;
		elem_width = 10;
		height = 100;
		left = 0;
		bottom = 0;
		r = g = b = a = 1;
		editable = 0;
		saveable = 0;
	} else {
		file >> ignore >> nlev; 
		lastlev = nlev - 1;
		file >> ignore >> left >> bottom;
		file >> ignore >> elem_width;
		file >> ignore >> height;
		file >> ignore >> r >> g >> b >> a;
		file >> ignore >> editable;
		file >> ignore >> saveable;
	}
		
	a0 = a / 4;
	bbox (left, bottom, left + nlev * elem_width, bottom + height);
	
	for (int i = 0; i < nlev; ++i) { 
		values.push_back (0);
		heights.push_back (0);
	}
	
	if (saveable) {
		int savings;
		file >> ignore >> savings;
		for (int i = 0; i < savings; ++i) {
			file >> values[i];
			heights[i] = values[i] * bbox.height;
		}
	}
	
	editing = edited = false;
	moving = false;
	
}

void levels::save () {
	extern string dotdin;
	ofstream file ((dotdin+name).c_str(), ios::out);
	if (file) {
		file << "num_levels " << nlev << endl;
		file << "lower_corner " << bbox.left << ' ' << bbox.bottom << endl;
		file << "element_width " << elem_width << endl;
		file << "height " << height << endl;
		file << "color " << r << ' ' << g << ' ' << b << ' ' << a << endl;
		file << "editable " << editable << endl;
		file << "saveable " << saveable << endl;
		if (saveable) {
			file << "savings " << nlev << ' ';
			for (int i = 0; i < nlev; ++i) file << values[i] << ' ';
			file << endl;
		} else {
			file << "savings 0" << endl;
		}
	} else cout << "couldnt save levels" << endl;
}

bool levels::handle_input () {

	static int dl, db, dr, dt;

	extern int mousex, mousey;
	extern viewport view;
	int mouseyy = view.ymax - mousey;
	edited = false;
	hover = false;
	if (inbox (bbox, mousex, mouseyy)) {
		
		hover = true;

		calc ();
				
		if (keypressed ('f')) {
			if (moving == false) {
				moving = true;
				dl = bbox.left - mousex;
				dr = bbox.right - mousex;
				db = bbox.bottom - mouseyy;
				dt = bbox.top - mouseyy;
			} else {
				moving = false;
			}
		}
		
		if (moving) {
			bbox (mousex + dl, mouseyy + db, mousex + dr, mouseyy + dt);
			edited = true;
		}
		
		if (editable) {
			
			if (keydown ('g')) {
				if (lev != -1) {
					set (lev, val, hgt);
					edited = true;
				}
			}
			
			if (keypressed ('h')) {
					clear ();
					if (lev != -1) {
						set (lev, val, hgt);
						edited = true;
					}
			}
			
			if (keydown ('c')) {
				if (lev != -1) {
					set (lev, 0, -1);
					edited = true;
				}
				
			}
			
		}
	
	}
	
	return edited;

}

void levels::calc () {
	
	extern viewport view;
	extern int mousex, mousey;
	int mouseyy = view.ymax - mousey;
	int dx = mousex - bbox.left;
	lev = dx / elem_width;
	if (lev > lastlev || lev < 0) lev = -1;
	hgt = mouseyy - bbox.bottom;
	val = hgt * bbox.height_1;
	clip (0.0F, val, 1.0F);
	clip (0, hgt, bbox.height);
	
}

void levels::clear () {
	/*lev = -1;
	val = 0; 
	hgt = 0;*/
	for (int i = 0; i < nlev; ++i) values[i] = heights[i] = 0;
}

void levels::draw () {
	
	glMatrixMode (GL_MODELVIEW);
		glPushMatrix ();
			glLoadIdentity ();
			glMatrixMode (GL_PROJECTION);
				glPushMatrix ();
					glLoadIdentity ();
						extern viewport view;
						glOrtho (0, view.xmax, 0, view.ymax, -1, 1);
							glEnable (GL_BLEND);
							glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
								int i = 0, x = bbox.left, ds = elem_width - 2;
								if (bbox.left < 0) {
									i = -bbox.left / elem_width;
									x = 0;
								} 
								for (;i < nlev; ++i) {
									int l = x + 1, r = x + ds;
									glColor4f (r, g, b, a0);
										glRecti (l, bbox.bottom, r, bbox.top);
									glColor4f (r, g, b, a);
										glRecti (l, bbox.bottom, r, bbox.bottom + heights[i]);
									x += elem_width;
									if (x > view.xmax) break;
								}
							glDisable (GL_BLEND);
							
							static char buf [256];
							sprintf (buf, "%d/%.3f", lev+1, lev > -1? values[lev]:-1);
							draw_string (buf, bbox.left, bbox.top + 4, 0);
							
				glPopMatrix ();
		glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
}

void levels::set (int i, float v, int h) {
		values[i] = v;
		if (h == -1) heights[i] = v * bbox.height; else heights[i] = h;
}

void levels::chkpos () {
	extern viewport view;
	if (!view.inside (bbox.left, bbox.bottom)) bbox.lower_corner (0, 0);
}
