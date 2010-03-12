#include <SDL/SDL_opengl.h>
#include "oscilloscope.h"
#include "console.h"
#include "font.h"
extern console cons;

// 0 init correct for oscilloscope
float oscilloscope::sample_t::lmin = 0, oscilloscope::sample_t::lmax = 0;
float oscilloscope::sample_t::rmin = 0, oscilloscope::sample_t::rmax = 0;

oscilloscope::oscilloscope () {
  viewr = 0;
  addr = 1;
  paused = false;
  visible = false;
}

void oscilloscope::toggle_pause () {
  paused = !paused;
  if (paused) cons << "scope is paused." << eol; else cons << "scope running." << eol;
}

void oscilloscope::toggle_visible () {
  visible = !visible;
  if (visible) cons << "scope is visible." << eol; else cons << "scope is hidden." << eol;
}

void oscilloscope::add_samples (float* out, int fpb) {
  for (int i = 0; i < fpb; ++i) {
		sample_t& sa = samples[addr];
		float l = *out++, r = *out++;
		sa.left = l; sa.right = r;
		sample_t::lmin = min (sa.left, sample_t::lmin);
		sample_t::lmax = max (sa.left, sample_t::lmax);
		sample_t::rmin = min (sa.right, sample_t::rmin);
		sample_t::rmax = max (sa.right, sample_t::rmax);
    if (++addr == MAX_SAMPLES) {
			addr = 0;
			sample_t::lmin = sample_t::lmax = sample_t::rmin = sample_t::rmax = 0; // ok for oscilloscope.
		}
    if (addr == viewr) if (++viewr == MAX_SAMPLES) viewr = 0;
  }
}

void oscilloscope::draw () {
  
  float x1 = 0, x2 = MAX_SAMPLES;
	
	glLineWidth (2);
	static char buf [256];
	sprintf (buf, "Min/Max: %0.3f, %0.3f", sample_t::lmin, sample_t::lmax);
	glColor3f (0, 1, 1);
	draw_string (buf, x1, sample_t::lmax * MAX_HEIGHT, 0);
	
	sprintf (buf, "Min/Max: %0.3f, %0.3f", sample_t::rmin, sample_t::rmax);
	glColor3f (1, 1, 0);
	draw_string (buf, x2, sample_t::rmax * MAX_HEIGHT, 0);
	glLineWidth (1);
	
  for (int i = 0, j = viewr; i < MAX_SAMPLES; ++i) {
    float l = samples[j].left, ly = l * MAX_HEIGHT;			  
    float lr = 0, lg = 1, lb = 1;
    if (l > 1) { lr = 1; lg = 0; lb = 0;}
    float r = samples[j].right, ry = r * MAX_HEIGHT;			  
    float rr = 1, rg = 1, rb = 0;
    if (r > 1) { rr = 1; rg = 0; rb = 0;}
    glBegin (GL_LINES);
      glColor3f (lr, lg, lb);
      glVertex2f (x1, 0);
      glVertex2f (x1, ly);
      glColor3f (rr, rg, rb);
      glVertex2f (x2, 0);
      glVertex2f (x2, ry);
    glEnd ();
    if (++j == MAX_SAMPLES) j = 0;
    ++x1; ++x2;
  }
	
}
