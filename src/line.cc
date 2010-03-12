#include <line.h>
#include <SDL/SDL_opengl.h>

void line::draw () {
  glBegin (GL_LINE_STRIP);
    for (int i = 0, j = points.size(); i < j; ++i) glVertex2f (points[i].x, points[i].y);
  glEnd ();
}

void line::set (int i, int x, int y) {
  points[i].x = x;
  points[i].y = y;
}

void line::insert (int i, int x, int y) {
  points.insert (points.begin() + i, point<int>(x,y));
}

int line::remove (int i) {
  points.erase (points.begin() + i);
  return points.size();
}
