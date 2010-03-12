#ifndef _MULTICURVE
#define _MULTICURVE

#include <string>
#include <fstream>
#include <vector>
#include "point.h"
#include "curve.h"
#include <iostream>
using namespace std;

typedef vector< point<float> > points_vector;

struct multi_curve {
	
	string name;
  	
	points_vector vertices;
	points_vector left_tangents;
	points_vector right_tangents;
	
	vector<curve> curv;
	vector<int> eval;
	
	float resolution;
	float r, g, b;
		
	static const int MAX_POINTS = 2048;
	crvpt points [MAX_POINTS];
	int npoints;
	
	multi_curve (const string& filename);
	multi_curve (const multi_curve& r);
	multi_curve ();
	multi_curve& operator= (const multi_curve& r);
	int num_vertices ();

	void add_vertex (float x, float y);
	void add_left_tangent (float x, float y);
	void add_right_tangent (float x, float y);

	bool set_vertex (int i, float x, float y);
	bool set_left_tangent (int i, float x, float y);
	bool set_right_tangent (int i, float x, float y);
	bool insert (float x, float y, float tx, float ty);
	bool remove (int i);
	void set_resolution (float d);

	void get_vertex (int i, float& x, float& y);
	void get_left_tangent (int i, float& x, float& y);
	void get_right_tangent (int i, float& x, float& y);
	float get_resolution ();

	void set_color ();
	void set_color (float rr, float gg, float bb);
	void get_color (float& rr, float& gg, float& bb) {
		rr = r; gg = g; bb = b;
	}
	void clear ();
	
	void force_eval ();
	void evaluate ();

	void load (const string& filename);
	void load (ifstream& file);
	void save (const string& filename);
	void save (ofstream& file);

	crvpt* get_points () {
		return points;
	}
	
	int num_points () {
		return npoints;
	}
	
	void copy (const multi_curve& r);				
};

void create_polyline (multi_curve& crv, const points_vector& pts);
void convert2_catmull_rom (multi_curve& crv, float tangent_size);
void convert2_polyline (multi_curve& crv);

#endif
