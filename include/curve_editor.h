#ifndef __curve_editor
#define __curve_editor

#include <vector>
#include <map>
#include <list>
using namespace std;

#include "curve.h"
#include "multi_curve.h"
#include "basic_editor.h"
#include "curve_listener.h"
#include "help.h"
#include "ui.h"
#include "mocap.h"

class curve_library;

struct hit_t {
	
	multi_curve* crv; // curve hit
  int crv_id; // curve index 
  enum {NONE=0, VERTEX, LEFT_TANGENT, RIGHT_TANGENT}; // things that can be hit in curve
  unsigned int what; // what was hit in curve
  int id; // id of that which was hit
  // tangent vectors from corresponding vertex
  point<float> left_tangent, right_tangent;
  float left_tangent_magnitude, right_tangent_magnitude;
  
  hit_t (multi_curve* cv = 0, int cid = -1, unsigned int w = NONE, int i = -1);
  void clear ();
  multi_curve* operator()();
  bool operator== (const hit_t& h) {return ((crv == h.crv) && (what == h.what) && (id == h.id));}
};

struct mouse_macro {

  // for applying mouse capture to hit vertex/tangent of hit curve
  
  mocap mo; // mouse capture data
  hit_t hit; // hit curve & vertex/tangent
  
  mouse_macro (const mocap& m, hit_t& h) : mo(m), hit(h) {}
  
};

struct undo_t {
  int i;
  multi_curve curve;
  undo_t (int ii, const multi_curve& mc) : i(ii), curve (mc) {}
};

typedef undo_t redo_t;

struct curve_info {

  multi_curve* curve;
  curve_listener* lisner;

  bool disabled;
  bool mark_segments;

  curve_info  (multi_curve* c, curve_listener* l, bool disabled = false);
  curve_info ();
  
};

struct curve_editor : basic_editor, ui {

  vector <curve_info> curveinfo; // edited curves
  
	// curve editor features
	//

	bool carry_tangents; // when vertex moves, their tangents move too.
	bool mirror_tangents; // when 1 tangent of a vertex moves, the other moves too.

	//
	// hit testing
	//
	
	vector <hit_t> hitlist;
	int hlid;
	hit_t pik;
	bool use_existing_hit;
  int handle_radius, hit_radius, hit_radius_2;
	void calc_hit_params (hit_t& h);
	void hittest (multi_curve* crv, int crv_id, const points_vector& points, unsigned int what);
	hit_t hittest ();
	void clear_hit (hit_t& h);

	// operations
  enum {DO_NOTHING = 0, MOVE_PICKED, MOVE_ALL, SCALE_ALL};
	hit_t movscl, ins, del, mir, rep;
	int todo;
	int curcrv;
		  
  // scratch curve 
	bool show_scratch_curve;
  points_vector win_scratch_points, curv_scratch_points;
  multi_curve scratch_curve;

	// load and save editor settings
	string settings_filename;
	
	// undo, redo
	//
	
	list <undo_t> undos;
	list <redo_t> redos;	
	void dodo (list<undo_t>& do1, list<undo_t>& do2, string mesg);
	
	// for copy & paste
	multi_curve copy;
	
	// curve library
	//
	curve_library* library;

  help helptext;
  
  vector<mouse_macro> macros;
  bool detach_mocap (hit_t& hit);
  void attach_mocap (hit_t& hit);
	
	bool label_cursor;
  
	void clearhit ();
	
	int fft_enabled;
	void dofft ();
	void draw_sine ();

	curve_editor (const string& settingsf, const string& helpf = "curve_editor.hlp");
	~curve_editor ();
			
	void load (const string& fname);
	void save ();
	
	bool handle_input ();
	
	
	// vertex ops
	bool move ();
	bool scale ();
	void insert ();
	void remove ();
	
	// curve ops
	void add (multi_curve* crv, curve_listener* lsnr, bool disabled = false);
	void clear ();
	int num_curves ();
	bool move (hit_t& hit, float x, float y);
	bool move (int);
	void replace ();
	void mirror (int whole_curve = 0);
	void resolution (int i, int dir);
	
	float rnd_miny, rnd_maxy;
	void randomise (); // randomise picked curve - useful for generating new waveforms, L & R response curves
	
	curve_info& get_curve_info (int i);
	multi_curve* get_curve (int i);
			
	// curve selection ops
	void pick (int i);
	void toggle (const hit_t& hit);
	void enable (const hit_t& hit);
	void enable_all ();
	bool one_curve_enabled ();

	void draw_common ();
	void draw_all ();
	void draw ();

	void clear_scratch_curve ();
	void draw_scratch_curve ();

	void draw_curve (multi_curve* crv);
	void draw_tangents (multi_curve* crv);
	void draw_vertices (multi_curve* crv);
	void draw_handle (const point<float>& p);
	void draw_tangent (const point<float>& p, const point<float>& t);

	void draw_curves ();
	void mark_curve_segments ();
	void draw_vertices ();
	void draw_tangents ();
	
	void attach_library (curve_library* lib);

	void enter ();
	void bg ();

	void set_handle_radius (int r);
	int get_handle_radius ();
	
	void apply_mocap ();
	
	void sine2curve ();

};
#endif
