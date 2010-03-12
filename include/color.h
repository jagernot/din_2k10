#ifndef __color
#define __color

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

struct color {
	float r, g, b;
	color& operator*= (float v) {
		r *= v;
		g *= v;
		b *= v;
		return *this;
	}
};

inline void hex2rgb (unsigned char hr, unsigned char hg, unsigned char hb, double& r, double& g, double& b) {
	const unsigned char ff = 0xff;
	const double ff1 = 1.0 / ff;
	r = hr * ff1;
	g = hg * ff1;
	b = hb * ff1;
}

inline void setcolor (unsigned char hr, unsigned char hg, unsigned char hb, float a = 1) {
	double r, g, b;
	hex2rgb (hr, hg, hb, r, g, b);
	glColor4f (r, g, b, a);
}

#endif
