#include "multi_curve.h"
#include "vector2d.h"
#include "container.h"
#include "random.h"
#include "console.h"
#include <iostream>
using namespace std;

extern console cons;

multi_curve::multi_curve (const string& filename) {
  load (filename);
}

multi_curve::multi_curve () : name ("noname"), resolution (1), npoints(0) {}

multi_curve& multi_curve::operator= (const multi_curve& r) {
	
	if (this != &r) copy (r);
	return *this;
}

multi_curve::multi_curve (const multi_curve& r) {	
		if (this != &r) copy (r);
}

void multi_curve::copy (const multi_curve& r) {
	
	vertices = r.vertices;
	left_tangents = r.left_tangents;
	right_tangents = r.right_tangents;
	curv = r.curv;
	eval = r.eval;
	resolution = r.resolution;
	
	// dont copy color
	name = r.name;
	
	npoints = r.npoints;
	for (int i = 0; i < multi_curve::MAX_POINTS; ++i) points[i] = r.points[i];
		
}

int multi_curve::num_vertices () {
  return vertices.size ();
}

void multi_curve::add_vertex (float x, float y) {
  vertices.push_back (point<float> (x, y));
	if (vertices.size() > 1) {
		curv.push_back (curve());
		eval.push_back (1);
	}
}

void multi_curve::add_left_tangent (float x, float y) {
  left_tangents.push_back (point<float> (x, y));
}

void multi_curve::add_right_tangent (float x, float y) {
  right_tangents.push_back (point<float> (x, y));
}

void multi_curve::get_vertex (int i, float& x, float& y) {
  point<float>& v = vertices[i];
  x = v.x; 
  y = v.y;
}

bool multi_curve::set_vertex (int i, float x, float y) {
  point<float>& v = vertices[i];
	if ((v.x != x) || (v.y != y)) {
		v.x = x; 
		v.y = y;
		eval[i] = 1;
		eval[max (0, i - 1)] = 1;
		return true;
	} else return false;
}

bool multi_curve::set_left_tangent (int i, float x, float y) {
  point<float>& lt = left_tangents [i];
	if ((lt.x != x) || (lt.y != y)) {
		lt.x = x;
		lt.y = y;
		int j = i - 1;
		if (j > -1) eval[j] = 1;
		return true;
	} else return false;
}

bool multi_curve::set_right_tangent (int i, float x, float y) {
  point<float>& rt = right_tangents[i];
	if ((rt.x != x) || (rt.y != y)) {
		rt.x = x;
		rt.y = y;
		if (i < (int)eval.size()) eval[i] = 1;
		return true;
	} else return false;
}

void multi_curve::get_left_tangent (int i, float& x, float& y) {
  point<float>& lt = left_tangents [i];
  x = lt.x;
  y = lt.y;
}

void multi_curve::get_right_tangent (int i, float& x, float& y) {
  point<float>& rt = right_tangents[i];
  x = rt.x;
  y = rt.y;
}

bool multi_curve::insert (float x, float y, float tx, float ty) {
  points_vector::iterator vter = ++vertices.begin (), lter = ++left_tangents.begin (), rter = ++right_tangents.begin ();
	vector<int>::iterator eter = eval.begin ();
	vector<curve>::iterator cter = curv.begin ();
  for (int i = 0, j = vertices.size() - 1; i < j; ++i, ++vter, ++lter, ++rter, ++eter, ++cter) {
    point<float>& lt = vertices[i];
    point<float>& rt = vertices[i + 1];
    if ((x >= lt.x) && (x <= rt.x)) {
      float ltx, lty; unit_vector (ltx, lty, x, y, lt.x, lt.y);
      float rtx, rty; unit_vector (rtx, rty, x, y, rt.x, rt.y);
      vertices.insert (vter, point<float>(x, y));
      left_tangents.insert (lter, point<float> (x + tx * ltx, y + ty * lty));
      right_tangents.insert (rter, point<float> (x + tx * rtx, y + ty * rty));
			curv.insert (cter, curve());
			eval[i] = 1;
			eval.insert (eter, 1);
      return true;
    } 
  }
	return false;
}

bool multi_curve::remove (int i) {
  // remove ith vertex and its tangents
  if (vertices.size () < 3) return false;
  erase (vertices, i);
  erase (left_tangents, i);
  erase (right_tangents, i);
	if (i == (int) eval.size()) {
		int j = i - 1;
		erase (eval, j);
		erase (curv, j);
	} else {
		erase (eval, i);
		erase (curv, i);
		if (--i > -1) eval[i] = 1;
	}
	return true;
}

void multi_curve::set_resolution (float d) {
  resolution = d;
	force_eval ();
}

void multi_curve::force_eval () { 
	for (int i = 0, j = eval.size(); i < j; ++i) eval[i] = 1;
}

float multi_curve::get_resolution () {
  return resolution;
}

void multi_curve::set_color () {
  static const float base = 0.1;
  static rnd<float> rd (0, 1. - base);
  r = base + rd (); g = base + rd (); b = base + rd ();
}

void multi_curve::set_color (float rr, float gg, float bb) {
  r = rr;
  g = gg;
  b = bb;
}

void multi_curve::evaluate () {
	
  int pid = 0; npoints = MAX_POINTS;
	int lastv = eval.size();
	for (int i = 0; i < lastv; ++i) {
		curve& crv = curv[i];
		if (eval[i]) {
			int j = i + 1;
			point<float>& v0 = vertices[i];
			point<float>& v1 = vertices[j];
			point<float>& rt0 = right_tangents[i];
			point<float>& lt1 = left_tangents[j];
			crv.vertex (0, v0.x, v0.y);
			crv.vertex (1, v1.x, v1.y);
			crv.tangent (0, rt0.x, rt0.y);
			crv.tangent (1, lt1.x, lt1.y);
			crv.set_distance (resolution);
			crv.eval ();
			eval[i] = 0;
		}
		for (list<crvpt>::iterator i = crv.pts.begin(), j = --crv.pts.end(); i != j; ++i) {
			if (pid >= MAX_POINTS) break;
			points[pid++] = *i;
		} 	
	}
  
  
  if (pid < MAX_POINTS) {
    point<float>& last_vertex = vertices [lastv];
    points[pid].x = last_vertex.x;
    points[pid].y = last_vertex.y;
		// .m? .t? init? - a bug waiting?
    npoints = ++pid;
  } else cout << "curve contains more than MAX_POINTS = " << MAX_POINTS << endl;
    
}

void multi_curve::clear () {
  vertices.clear ();
  left_tangents.clear ();
  right_tangents.clear ();
	eval.clear ();
	curv.clear ();
  npoints = 0;
}

void multi_curve::load (const string& filename) {
  extern string dotdin;
  ifstream file ((dotdin + filename).c_str (), ios::in);
  if (!file) return;
  load (file);
  
}

void multi_curve::load (ifstream& file) {
  
  string ignore;
  
	file >> ignore >> name;
	
  int num_vertices;
  file >> ignore >> num_vertices;
	
	clear ();
      
  for (int i = 0; i < num_vertices; ++i) {
    
    float x, y;
    file >> ignore >> x >> y;
		add_vertex (x, y);
    file >> ignore >> x >> y;
		add_left_tangent (x, y);
    file >> ignore >> x >> y;
		add_right_tangent (x, y);
    
  }
  
  file >> ignore >> resolution;
  
  file >> ignore >> r >> g >> b;
  
  evaluate ();
  
  
}

void multi_curve::save (const string& filename) {
  extern string dotdin;
  ofstream file ((dotdin + filename).c_str (), ios::out);
  if (!file) return;
  save (file);
}

void multi_curve::save (ofstream& file) {

  string ignore;
  int num_vertices = vertices.size ();
    
	file << "name " << name << eol;
	
  file << "num_vertices " << num_vertices << eol;
  
  for (int i = 0; i < num_vertices; ++i) {
    point<float>& v = vertices[i];
    point<float>& lt = left_tangents[i];
    point<float>& rt = right_tangents[i];
    file << "vertex " << v.x << ' ' << v.y << eol;
    file << "left_tangent " << lt.x << ' ' << lt.y << eol;
    file << "right_tangent " << rt.x << ' ' << rt.y << eol;
  }
  file << "curve_resolution " << resolution << eol;
  file << "color " << r << ' ' << g << ' ' << b << eol;
}

void create_polyline (multi_curve& crv, const points_vector& pts) {
      
  int npts = pts.size ();
  
  if (npts < 2) return;  

  crv.clear ();
  
  for (int i = 0; i < npts; ++i) {
    
    float xi = pts[i].x, yi = pts[i].y;
    crv.add_vertex (xi, yi);
    crv.add_left_tangent (xi, yi);
    crv.add_right_tangent (xi, yi);
    
  }
  
  crv.evaluate ();
  
}

void convert2_polyline (multi_curve& crv) {
  
  for (int i = 0, j = crv.vertices.size(); i < j; ++i) {
    point<float>& v = crv.vertices[i];
		crv.set_left_tangent (i, v.x, v.y);
		crv.set_right_tangent (i, v.x, v.y);
  }
  
  crv.evaluate ();
  
}

void convert2_catmull_rom (multi_curve& crv, float tangent_size) {
  
  int npts = crv.num_vertices ();
  
  if (npts < 2) return;
  
  int last = npts - 1;
  point<float>& p0 = crv.vertices[0];
  point<float>& p1 = crv.vertices[1];
  point<float>& pl = crv.vertices[last];
  point<float>& pl1 = crv.vertices[last-1];

  // set tangents for 1st vertex
  float dx, dy; direction (dx, dy, p0.x, p0.y, p1.x, p1.y);
  float tx = tangent_size * dx, ty = tangent_size * dy;
  crv.set_left_tangent (0, p0.x - tx, p0.y - ty);
  crv.set_right_tangent (0, p0.x + tx, p0.y + ty);
  
  // set tangents for last vertex
  direction (dx, dy, pl.x, pl.y, pl1.x, pl1.y);
  tx = tangent_size * dx; ty = tangent_size * dy;
  crv.set_left_tangent (last, pl.x + tx, pl.y + ty);
  crv.set_right_tangent (last, pl.x - tx, pl.y - ty);
  
  // set left, right tangent for inbetween vertices
  for (int i = 1; i < last; ++i) {
    int l = i - 1, r = i + 1;
    point<float>& pi = crv.vertices[i];
    point<float>& pl = crv.vertices[l];
    point<float>& pr = crv.vertices[r];
    direction (dx, dy, pl.x, pl.y, pr.x, pr.y);
    crv.set_left_tangent (i, pi.x - tangent_size * dx, pi.y - tangent_size * dy);
    crv.set_right_tangent (i, pi.x + tangent_size * dx, pi.y + tangent_size * dy);
  }
  
  crv.evaluate ();
  
}
