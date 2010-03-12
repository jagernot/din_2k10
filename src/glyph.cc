#include <glyph.h>

glyph::glyph (const vector<line>& vl) : lines(vl) {}

void glyph::find_width_height () {
  
  if (int nlines = lines.size ()) {
    
    int r, b, l, t; 
    r = b = l = t = 0;
    vector< point<int> >& points = lines[0].points;
    if (points.size()) {
      point<int>& p = points[0];
      r = l = p.x;
      b = t = p.y;
    }
    
    for (int i = 0; i < nlines; ++i) {
      vector< point<int> >& points = lines[i].points;
      for (int p = 0, q = points.size(); p < q; ++p) {
        point<int>& pt = points[p];
        if (pt.x < l) l = pt.x; else if (pt.x > r) r = pt.x;
        if (pt.y < b) b = pt.y; else if (pt.y > t) t = pt.y;
      }
    }
    
    width = r - l;
    height = t - b;
    
  }
  
}
