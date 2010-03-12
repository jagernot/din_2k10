#include <textboard.h>
#include <SDL/SDL_opengl.h>
#include <font.h>

void textboard::draw () {
	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
	glLoadIdentity ();
	extern viewport view;
	glOrtho (0, view.xmax, 0, view.ymax, -1, 1);
	for (int i = 0, j = texts.size(); i < j; ++i) {
		text& t = texts[i];
		glColor3f (t.r, t.g, t.b);
		draw_string (t.txt, t.x, t.y);
	}
	glPopMatrix ();
}
