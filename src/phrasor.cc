#include "phrasor.h"
#include <SDL/SDL_opengl.h>

void phrasor::draw () {
	int ls = 25;
	const float c = 0.8; 
	glColor3f (c, c, c);
	int x = data[cur].win_mousex, y = data[cur].win_mousey;
	glBegin (GL_LINES);
		glVertex2i (x - ls, y);
		glVertex2i (x + ls, y);
		glVertex2i (x, y - ls);
		glVertex2i (x, y + ls);
	glEnd ();
}



