#ifndef __line
#define __line

#include <vector>
#include <point.h>
using namespace std;

struct line {
  vector< point<int> > points;
  line () {}
  line (const vector< point<int> >& pts) : points(pts) {}
	line (const vector< point<float> >& pts) {
		points.clear ();
		for (unsigned int i = 0, j = pts.size(); i < j; ++i) {
			points.push_back (point<int>(pts[i].x, pts[i].y));
		}
	}
  void draw ();
  void set (int i, int x, int y);
  void insert (int i, int x, int y);
  int remove (int i);
};

#endif
