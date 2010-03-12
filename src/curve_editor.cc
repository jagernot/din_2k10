#include <input.h>
#include <curve_editor.h>
#include <curve_library.h>
#include <vector2d.h>
#include <iostream>
#include <console.h>
#include <font.h>
#include <utils.h>
#include <random.h>
#include <fft.h>
#include <sine_mixer.h>
using namespace std;

extern console cons;

console& operator<< (console& c, const hit_t& h) {
  const string what [] = {" nothing", " vertex", " left tangent", " right tangent"};
  c << h.crv->name << what[h.what] << ' ' << h.id;
  return c;
}

curve_editor::curve_editor (const string& settingsf, const string& helpf) : helptext (helpf) {
	
  use_existing_hit = false;
	hlid = 0;
	
	todo = DO_NOTHING;
	curcrv = 0;
	
	rnd_miny = -1; rnd_maxy = 1; // default suitable for random waveform generation
	
	fft_enabled = 0;
  
	load  (settingsf);
	
}

curve_editor::~curve_editor () {
  save ();
}

void curve_editor::load (const string& fname) {

  // load settings from file
	
	extern string dotdin; 
  ifstream file ((dotdin + fname).c_str (), ios::in);
  if (!file) {
    cout << "cannot open " << fname << endl;
    return;
  } else cout << "reading settings from " << fname << endl;

  settings_filename = fname;

  basic_editor::load (file);
  
  string ignore;
  
  int ct; file >> ignore >> ct;
  carry_tangents = (bool) ct;

  int mt; file >> ignore >> mt;
  mirror_tangents = (bool) mt;
      
  int hr; file >> ignore >> hr;
  set_handle_radius (hr);
	
	file >> ignore >> rnd_miny >> rnd_maxy;
	
	file >> ignore >> fft_enabled;

  cout << "read settings from " << fname << endl;

}

void curve_editor::save () {
	
	// save settings to file

	extern string dotdin;
  ofstream file ((dotdin + settings_filename).c_str(), ios::out);
  if (!file) {
    cout << "cannot save " << settings_filename << endl;
    return;
  }
  
  basic_editor::save (file);
  
  file << "carry_tangents " << carry_tangents << eol;
  file << "mirror_tangents " << mirror_tangents << eol;
  file << "curve_handle_radius " << handle_radius << eol;
	file << "rnd_miny_maxy " << rnd_miny << ' ' << rnd_maxy << eol;
	file << "fft_enabled " << fft_enabled << eol;

}

bool curve_editor::handle_input () {
	
	// fft only on waveform editor
	if (fft_enabled) {
		extern sine_mixer sinemixer;
		extern fft fft0;
		if (sinemixer.handle_input () || fft0.lev.handle_input ()) return true;
	}
	
	basic_editor::handle_input ();
		
	// help
	if (keypressed (SDLK_F1)) helptext();
	
	if (keypressed (SDLK_F8)) { // apply mouse capture
		hit_t hit = hittest ();
		if (hit() && mocap0()) {
			undos.push_back (undo_t (hit.crv_id, *hit.crv));
			macros.push_back (mouse_macro (mocap0, hit));
			cons << "assigned mouse capture to " << hit << eol;
		}
	}
	
	if (keydown (SDLK_F9)) { // remove mouse capture
		hit_t hit = hittest ();
		if (hit()) {
			for (vector<mouse_macro>::iterator i = macros.begin(), j = macros.end(); i != j; ++i) {
				mouse_macro& m = *i;
				if (hit == m.hit) {
					macros.erase (i);
					cons << "detached mouse capture from " << hit << eol;
					break;
				}
			}
		}
	}
		
	if (keypressed ('c')) { 
		if (keydown (SDLK_LCTRL)) {
			hit_t hit = hittest ();
			if (hit()) {
				copy = *hit.crv;
				cons << console::green << "copied curve." << eol;
			}
		} else {
			carry_tangents = !carry_tangents;
			if (carry_tangents) cons << console::yellow << "vertices carry their tangents." << eol; else cons << console::yellow << "vertices desert their tangents" << eol;
		}
	}

	if (keypressed ('m')) {
		mirror_tangents = !mirror_tangents;
		if (mirror_tangents) cons << console::yellow << "tangents are mirrored." << eol; else cons << console::yellow << "tangents are not mirrored." << eol;
	}

	// mark curve segments
	if (keypressed (',')) {
		hit_t hit = hittest ();
		if (hit()) {
			curve_info& ci = curveinfo [hit.crv_id];
			ci.mark_segments = !ci.mark_segments;
		}
	}

	// curve color
	if (keypressedd (';')) {
		hit_t hit = hittest (); 
		if (hit()) hit.crv->set_color ();
	}
	
	// move vertex/tangent
	if (keypressed ('f')) {
		if (todo) {
			todo = DO_NOTHING;
			clear_hit (movscl);
		} else {
			movscl = hittest ();
			if (movscl()) {
				if (keydown (SDLK_LSHIFT)) todo = MOVE_ALL; else if (keydown (SDLK_LCTRL)) todo = SCALE_ALL; else todo = MOVE_PICKED;
				undos.push_back (undo_t (movscl.crv_id, *movscl.crv));
			}
		}
	} else {
		
		bool result = false;
		
		switch (todo) {
			case MOVE_PICKED:
				result = move ();
				break;
			case MOVE_ALL:
				result = move (1);
				break;
			case SCALE_ALL:
				result = scale ();
				break;
		}
		
	}
	
	// insert vertex
	if (keypressed ('i')) {
		if (!ins()) ins = hittest(); else {
			insert ();
			clear_hit (ins);
		}
	}
	
	if (keypressed ('v')) {
		if (keydown (SDLK_LCTRL)) {
			// paste copied curve
			hit_t hit = hittest ();
			if (hit()) {
				curve_info& ci = curveinfo [hit.crv_id];        
				if (copy.num_vertices()) {
					undos.push_back (undo_t (hit.crv_id, *hit.crv));
					*hit.crv = copy;
					ci.lisner->edited (hit.crv_id);
					if (fft_enabled) dofft ();
					cons << console::green << "pasted curve." << eol;
				} else cons << console::red << "no copy curve." << eol;
			} else cons << console::red << "pick a curve to paste." << eol;
		} else {
			// delete vertex
			del = hittest ();
			if (del()) {
				remove ();
				clear_hit (del);
			}
		}
	} 
	
	if (keypressedd ('t')) { 
		// mirror curve/vertex about x-axis
		mir = hittest ();
		if (mir()) {
			if (keydown (SDLK_LSHIFT)) 
				mirror (1);  // whole curve
			else 
				mirror (); // only hit vertex
			clear_hit (mir);	
		}
	}
	
	// replace curve
	//
	if (keypressed ('g')) {
		if (rep()) {
			show_scratch_curve = true;
			float sx, sy; snap (sx, sy);
			win_scratch_points.push_back (point<float> (sx, sy));
			float curvx, curvy; win2obj (sx, sy, curvx, curvy);
			curv_scratch_points.push_back (point<float>(curvx, curvy));
		} else {
			rep = hittest ();
			clear_scratch_curve ();
		}
	}

	if (keypressed ('h')) {
		if (rep()) {
			replace ();
			clear_hit (rep);
			show_scratch_curve = false;
		}
	}

	if (keypressed ('k')) {
		hit_t hit = hittest ();
		if (hit()) {
			curve_info& ci = curveinfo[hit.crv_id];
			multi_curve& crv = *hit.crv;
			undos.push_back (undo_t (hit.crv_id, crv));
			const point<float>& obj = get_obj_chunk ();
			if (keydown (SDLK_LSHIFT)) { // unfurl tangents of hit vertex
				float vx, vy; crv.get_vertex (hit.id, vx, vy);
				crv.set_left_tangent (hit.id, vx - obj.x, vy);
				crv.set_right_tangent (hit.id, vx + obj.x, vy);
				crv.evaluate ();
			} else convert2_catmull_rom (crv, max (obj.x, obj.y));
			ci.lisner->edited (hit.crv_id);
			if (fft_enabled) dofft ();
		}
	}

	if (keypressed ('l')) {
		hit_t hit = hittest ();
		if (hit ()) {
			multi_curve& crv = *hit.crv;
			undos.push_back (undo_t (hit.crv_id, crv));
			if (keydown (SDLK_LSHIFT)) { // fold tangents of hit vertex
				float vx, vy; crv.get_vertex (hit.id, vx, vy);
				crv.set_left_tangent (hit.id, vx, vy);
				crv.set_right_tangent (hit.id, vx, vy);
				crv.evaluate ();
			} else convert2_polyline (crv);
			curveinfo[hit.crv_id].lisner->edited (hit.crv_id);
			if (fft_enabled) dofft ();
		}
	}

	if (keypressedd ('[')) {
		hit_t hit = hittest (); 
		if (hit()) {
			resolution (hit.crv_id, +1);
		}
	}

	if (keypressedd (']')) {
		hit_t hit = hittest ();
		if (hit()) {
			resolution (hit.crv_id, -1);
		}
	}
	
	if (keypressedd ('z') && !movscl()) {
		if (keydown (SDLK_LSHIFT)) 
			dodo (redos, undos, "no more redos.");
		else 
			dodo (undos, redos, "no more undos.");
	}
		
	if (keypressed ('r')) {
		use_existing_hit = false;
		hittest ();
		if (pik()) {
			pick (pik.crv_id);
			use_existing_hit = true;
		}
	}
	
	if (keypressed (SDLK_LEFT)) {
		int n = hitlist.size (); 
		if (n) {
			int l = n - 1;
			if (--hlid < 0) hlid = l;
			pik = hitlist[hlid];
			calc_hit_params (pik);
			curveinfo [pik.crv_id].lisner->selected (pik.crv_id);
			use_existing_hit = true;
		}
	}
	
	if (keypressed (SDLK_RIGHT)) {
		int n = hitlist.size();
		if (n) {
			int l = n - 1;
			if (++hlid > l) hlid = 0;
			pik = hitlist[hlid];
			calc_hit_params (pik);
			curveinfo [pik.crv_id].lisner->selected (pik.crv_id);
			use_existing_hit = true;
		}
	} 
	
	if (keypressed (SDLK_UP)) {
		hitlist.clear ();
		use_existing_hit = false;
		cons << console::green << "cleared hitlist" << eol;
	}
		
	if (library) {
		if (library->num_curves()) { 
			
			if (keypressed ('u')) library->move(-1);
			if (keypressed ('j')) library->move(+1);
			
			if (keypressed ('7')) {
				if (pik()); else pik = hit_t (0);
				library->insert (*get_curve (pik.crv_id));
			}

			if (keypressed ('8')) {
				if (pik()); else pik = hit_t (0);
				library->replace (*get_curve (pik.crv_id));
			}
			
			if (keypressedd ('9')) {
				if (pik()); else pik = hit_t (curveinfo[0].curve, 0);
				multi_curve& crv = *pik.crv;
				undos.push_back (undo_t (pik.crv_id, crv));
				crv = library->prev();
				curveinfo[pik.crv_id].lisner->edited (pik.crv_id);
				if (fft_enabled) dofft ();
				
			}
			
			if (keypressedd ('0')) { 
				if (pik()); else pik = hit_t (curveinfo[0].curve, 0);
				multi_curve& crv = *pik.crv;
				undos.push_back (undo_t (pik.crv_id, crv));
				crv = library->next ();
				curveinfo[pik.crv_id].lisner->edited (pik.crv_id);
				if (fft_enabled) dofft ();
			}
			
			if (keypressed ('-')) {
				library->del ();
			}
			
		}
		
		if (keypressed ('=')) {
			if (pik()); else pik = hit_t (0);
			curve_info& ci = curveinfo [pik.crv_id];
			library->add (*ci.curve);
		}
		
		
	}
		
	if (keypressedd ('\'')) {
		int h = get_handle_radius ();
		set_handle_radius (--h);
	}
	
	if (keypressedd ('\\')) {
		int h = get_handle_radius ();
		set_handle_radius (++h);
	}
	
	if (keypressed ('/')) { // enable/disable curves
	
		if (one_curve_enabled()) 
			enable_all (); 
		else {
			hit_t hit = hittest ();
			if (hit()) enable (hit); else if (pik()) enable (pik);
		}
		
	}
	
	if (keypressed ('.')) {
		hit_t hit = hittest ();
		if (hit()) toggle (hit);
	}
	/*
	if (keypressedd ('o')) {
		if (--curcrv < 0) curcrv = num_curves() - 1;
		pick (curcrv);
	} else if (keypressedd ('p')) {
		if (++curcrv >= num_curves()) curcrv = 0;
		pick (curcrv);
	}*/
	
	
	if (keypressed (SDLK_F7)) {
		if (fft_enabled && num_curves()) dofft ();
	}
	
	if (keypressed (SDLK_RCTRL)) sine2curve ();
	
	save_win_mouse ();
  
  return true;
	
}

void curve_editor::sine2curve () {
	if (fft_enabled) {
		extern sine_mixer sinemixer;
		multi_curve& crv = *curveinfo[0].curve;
		undos.push_back (undo_t (0, crv));
		sinemixer.make_curve (crv);
		dofft ();
	}
}

void curve_editor::apply_mocap () {
	 
  for (int i = 0, j = macros.size(); i < j; ++i) {
    
    mouse_macro& m = macros[i];
    hit_t& h = m.hit;
    float dx, dy; 
    if (m.mo.get(dx, dy) == true) move (h, dx, dy);
    
  }
  
}

void curve_editor::pick (int i) {
  int ncurves = curveinfo.size ();
  if (i > -1 && i < ncurves) {
    curve_info& ci = curveinfo[i];
		cout << "picked curve " << i << eol;
    ci.lisner->selected (i);
  }
}

void curve_editor::toggle (const hit_t& hit) {

  // enable/disable hit curve
  curve_info& ci = curveinfo[hit.crv_id];
  ci.disabled = !ci.disabled;
  curve_listener* cl = ci.lisner;
  cl->disabled (hit.crv_id, ci.disabled);
  pik = hit;
  cl->selected (hit.crv_id);
  
}

void curve_editor::enable_all () {
  
  // enable all curves
  for (int i = 0, j = curveinfo.size(); i < j; ++i) {
    curve_info& ci = curveinfo[i];
    curve_listener* cl = ci.lisner;
    ci.disabled = false;
    cl->disabled (i, false);
  }
  
}

void curve_editor::enable  (const hit_t& hit) {
  
  // enable hit curve, disable all other curves
  for (int i = 0, j = curveinfo.size(); i < j; ++i) {
    curve_info& ci = curveinfo[i];
    curve_listener* cl = ci.lisner;
    if (i != hit.crv_id) {
      ci.disabled = true;
      cl->disabled (i, true); 
    } else {
      ci.disabled = false;
      cl->disabled (i, false);
      pik = hit_t (ci.curve, i);
      cl->selected (i);
    }
  }
  
}

bool curve_editor::one_curve_enabled () {
  int nenabl = 0;
  for (int i = 0, j = curveinfo.size(); i < j; ++i) {
    curve_info& ci = curveinfo[i];
    if (!ci.disabled) if (++nenabl > 1) return false;
  }
  return true;
}

void curve_editor::calc_hit_params (hit_t& hit) {
		
	if (hit.crv == 0) return;
	
	multi_curve* crv = hit.crv;
	float vx, vy, lx, ly, rx, ry;
	int id = hit.id;
	crv->get_vertex (id, vx, vy);
	crv->get_left_tangent (id, lx, ly);
	crv->get_right_tangent (id, rx, ry);
	direction (hit.left_tangent.x, hit.left_tangent.y, vx, vy, lx, ly);
	direction (hit.right_tangent.x, hit.right_tangent.y, vx, vy, rx, ry);
	hit.left_tangent_magnitude = magnitude (hit.left_tangent.x, hit.left_tangent.y);
	hit.right_tangent_magnitude = magnitude (hit.right_tangent.x, hit.right_tangent.y);
	
	cons << console::green << "picked " << hit << " [" << hlid+1 << '/' << hitlist.size() << ']' << eol;
	
}

void curve_editor::hittest (multi_curve* crv, int crv_id, const points_vector& points, unsigned int what) {
  
  float wx, wy; get_win_mouse (wx, wy);
  for (int i = 0, j = points.size (); i < j; ++i) {
    const point<float>& v = points[i];
    int vdx, vdy; obj2win (v, vdx, vdy);
    float d2 = distance2<float> (vdx, vdy, wx, wy);
    if (d2 <= hit_radius_2) hitlist.push_back (hit_t (crv, crv_id, what, i));
  }
}

hit_t curve_editor::hittest () {
	  
  hit_t hit;
	
	if (use_existing_hit) {
		use_existing_hit = false;
		return pik; 
	} else {
		hitlist.clear ();
		hlid = 0;
	}
	
  for (int i = 0, j = curveinfo.size (); i < j; ++i) {
    curve_info& ci = curveinfo[i];
    multi_curve* crv = ci.curve;
		hittest (crv, i, crv->vertices, hit_t::VERTEX);
		hittest (crv, i, crv->left_tangents, hit_t::LEFT_TANGENT);
		hittest (crv, i, crv->right_tangents, hit_t::RIGHT_TANGENT);
  }
	
	int n = hitlist.size();
	if (n) {
		pik = hitlist[0];
		calc_hit_params (pik);
	} else pik = hit_t ();
	
	return pik;
	
}

void curve_editor::set_handle_radius (int r) {
  if (r < 1) r = 1;
  handle_radius = r;
  hit_radius = handle_radius;
  hit_radius_2 = hit_radius * hit_radius;
}

int curve_editor::get_handle_radius () {
  return handle_radius;
}

bool curve_editor::move (hit_t& hit, float x, float y) {
  
  curve_info& ci = curveinfo [hit.crv_id];
  multi_curve* crv = ci.curve;
	
	bool need_eval = false;
  switch (hit.what) {
    case hit_t::VERTEX:
        if (carry_tangents) {
          crv->set_left_tangent (hit.id, x + hit.left_tangent.x, y + hit.left_tangent.y);
          crv->set_right_tangent (hit.id, x + hit.right_tangent.x, y + hit.right_tangent.y);
        }
				
        need_eval = crv->set_vertex (hit.id, x, y);

      break;
    case hit_t::LEFT_TANGENT:

        if (mirror_tangents) {
          float vx, vy, lvx, lvy;
          crv->get_vertex (hit.id, vx, vy);
          unit_vector (lvx, lvy, vx, vy, x, y);
					crv->set_right_tangent (hit.id, vx - hit.right_tangent_magnitude * lvx, vy - hit.right_tangent_magnitude * lvy);
					
        }
        
				need_eval = crv->set_left_tangent (hit.id, x, y);

      break;
			
    case hit_t::RIGHT_TANGENT:

        if (mirror_tangents) {
          float vx, vy, rvx, rvy;
          crv->get_vertex (hit.id, vx, vy);
          unit_vector (rvx, rvy, vx, vy, x, y);
          crv->set_left_tangent (hit.id, vx - hit.left_tangent_magnitude * rvx, vy - hit.left_tangent_magnitude * rvy);
        }
        
				need_eval = crv->set_right_tangent (hit.id, x, y);

      break;

  }

  if (need_eval) {
		crv->evaluate(); 
		if (fft_enabled) dofft();
		ci.lisner->edited (hit.crv_id);
	}
	
	return need_eval;
}

bool curve_editor::move () {
  float sx, sy; snap (sx, sy);
  float cx, cy; win2obj (sx, sy, cx, cy);
  return move (movscl, cx, cy);
}

bool curve_editor::move (int) {
  
  float sx, sy; snap (sx, sy);

  float cx, cy; win2obj (sx, sy, cx, cy);

  curve_info& ci = curveinfo [movscl.crv_id];
  multi_curve* crv = ci.curve;
  
  float vx, vy;
  float dx, dy;

  switch (movscl.what) {
    case hit_t::VERTEX:
        crv->get_vertex (movscl.id, vx, vy);
        break;
    case hit_t::LEFT_TANGENT:
        crv->get_left_tangent (movscl.id, vx, vy);
        break;
    case hit_t::RIGHT_TANGENT:
        crv->get_right_tangent (movscl.id, vx, vy);
        break;
  }
  
  dx = cx - vx;
  dy = cy - vy;
  
	bool result = false;
  for (int i = movscl.id, j = crv->num_vertices (); i < j; ++i) {
    crv->get_vertex (i, vx, vy);
    result |= crv->set_vertex (i, vx + dx, vy + dy);
    crv->get_left_tangent (i, vx, vy);
    result |= crv->set_left_tangent (i, vx + dx, vy + dy);
    crv->get_right_tangent (i, vx, vy);
    result |= crv->set_right_tangent (i, vx + dx, vy + dy);
  }
	
	if (result) {
		crv->evaluate(); 
		if (fft_enabled) dofft();
		ci.lisner->edited (movscl.crv_id);
	}
	
	return result;
	
}

bool curve_editor::scale () {
    
  curve_info& ci = curveinfo [movscl.crv_id];
  multi_curve* crv = ci.curve;
  
  float vx, vy;
  float dx, dy;
    
  crv->get_vertex (movscl.id, vx, vy); 
  float dwx = win_mousex - prev_win_mousex;
  float dwy = win_mousey - prev_win_mousey;
  
  const float scaler = 0.005;
  
  int both = is_snapx() && is_snapy ();
  if (both == false) {
		if (is_snapx()) dwy = 0; else if (is_snapy()) dwx = 0;
	}
  if (dwx == 0) dx = 1; else dx = 1 + dwx * scaler;
  if (dwy == 0) dy = 1; else dy = 1 + dwy * scaler;
      
	bool result = false;
  for (int i = movscl.id + 1, j = crv->num_vertices (); i < j; ++i) {
    crv->get_vertex (i, vx, vy);
    result |= crv->set_vertex (i, vx * dx, vy * dy);
    crv->get_left_tangent (i, vx, vy);
    result |= crv->set_left_tangent (i, vx * dx, vy * dy);
    crv->get_right_tangent (i, vx, vy);
    result |= crv->set_right_tangent (i, vx * dx, vy * dy);
  }
  
	if (result) {
		crv->evaluate(); 
		if (fft_enabled) dofft();
		ci.lisner->edited (movscl.crv_id);
	}
	
	return result;
	
}

void curve_editor::remove () {
  
  curve_info& ci = curveinfo [del.crv_id];
  multi_curve* crv = ci.curve;
  
  undos.push_back (undo_t (del.crv_id, *crv));
  
  if (crv->remove (del.id)) {
		crv->evaluate(); 
		if (fft_enabled) dofft();
		ci.lisner->edited (del.crv_id);
	} else undos.pop_back ();

}

void curve_editor::insert () {

  float sx, sy; snap (sx, sy);
  float cx, cy; win2obj (sx, sy, cx, cy);
  

  curve_info& ci = curveinfo [ins.crv_id];
  multi_curve* crv = ci.curve;
  undos.push_back (undo_t (ins.crv_id, *crv));
  
  const point<float>& obj = get_obj_chunk ();
  if (crv->insert (cx, cy, obj.x, obj.y)) {
		crv->evaluate(); if (fft_enabled) dofft();
		ci.lisner->edited (ins.crv_id);
	} else undos.pop_back ();

}

void curve_editor::replace () {
    
  curve_info& ci = curveinfo [rep.crv_id];
  multi_curve& crv = *ci.curve;
  undos.push_back (undo_t (rep.crv_id, crv));
  
  create_polyline (crv, curv_scratch_points);
  const point<float>& obj = get_obj_chunk ();
  convert2_catmull_rom (crv, max (obj.x, obj.y));
  
  ci.lisner->edited (rep.crv_id);
	if (fft_enabled) dofft ();
  
  
}

void curve_editor::mirror (int whole_curve) {
  
  // mirrors vertex (or curve) about horizontal - useful when editing waveforms

  curve_info& ci = curveinfo [mir.crv_id];
  multi_curve* crv = ci.curve;
  undos.push_back (undo_t (mir.crv_id, *crv));
  float xx, yy; 
  if (whole_curve) {
    int num_vertices = crv->num_vertices ();
    for (int i = 0; i < num_vertices; ++i) {
      crv->get_vertex (i, xx, yy); crv->set_vertex (i, xx, -yy);
      crv->get_left_tangent (i, xx, yy); crv->set_left_tangent (i, xx, -yy);
      crv->get_right_tangent (i, xx, yy); crv->set_right_tangent (i, xx, -yy);
    }
    
		crv->evaluate(); if (fft_enabled) dofft();
    ci.lisner->edited (mir.crv_id);
    
  } else {
    switch (mir.what) {
      case hit_t::VERTEX:
        crv->get_vertex (mir.id, xx, yy);
        break;
      case hit_t::LEFT_TANGENT:
        crv->get_left_tangent (mir.id, xx, yy);
        break;
      case hit_t::RIGHT_TANGENT:
        crv->get_right_tangent (mir.id, xx, yy);
        break;
    }
    move (mir, xx, -yy);
  }
  
}

void curve_editor::resolution (int i, int dir) {
  
  curve_info& ci = curveinfo [i];
  multi_curve* crv = ci.curve;
  float res = crv->get_resolution();
  float newres = res + dir * get_obj_resolution ();
  if (newres > 0) {
    crv->set_resolution (newres);
		crv->evaluate(); if (fft_enabled) dofft();
    ci.lisner->edited (i);
  }
  
}

void curve_editor::draw () {
  
  project ();
    draw_common ();
    draw_all ();
		if (fft_enabled) draw_sine ();
  unproject ();

	if (fft_enabled) {
		extern fft fft0; fft0.lev.draw ();
		extern sine_mixer sinemixer; sinemixer.draw ();
	}
  
}

void curve_editor::draw_sine () {
	
	extern sine_mixer sinemixer;
	int n = sinemixer.norm.size ();
	float ox = 0, oy = 0, dx = 1. / (n - 1);
	glColor3f (0, 0, 1);
	glBegin (GL_LINE_STRIP);
	int wx, wy;
	for (int i = 0; i < n; ++i) {
		oy = sinemixer.norm[i];
		obj2win (ox, oy, wx, wy);
		glVertex2f (wx, wy);
		ox += dx;
	}
	glEnd ();
}

void curve_editor::draw_common () {
  basic_editor::draw ();
  mark_curve_segments ();
  if (show_scratch_curve) draw_scratch_curve ();
}

void curve_editor::draw_scratch_curve () {
  
  glColor3f (1, 1, 0.7);
  glBegin (GL_LINE_STRIP);
  
  float x, y; snap (x, y);
  for (int i = 0, j = win_scratch_points.size (); i < j; ++i) {
    const point<float>& p = win_scratch_points[i];
    x = p.x; y = p.y;
    glVertex2f (x, y);
  }

  glVertex2f (x, y);

  float sx, sy; snap (sx, sy);
  glVertex2f (sx, sy);

  glEnd ();

}

void curve_editor::draw_all () {
  for (int i = 0, j = curveinfo.size(); i < j; ++i) {
    curve_info& ci = curveinfo[i];
    multi_curve* crv = ci.curve;
    if (ci.disabled) {
      glColor3f (0.2, 0.2, 0.2); 
      draw_curve (crv);
      draw_vertices (crv);
    } else  {
			glColor3f (crv->r, crv->g, crv->b);
			if ((i == pik.crv_id) && (j > 1)) glLineWidth (3);
      draw_curve (crv);
      draw_vertices (crv);
      draw_tangents (crv);
			glLineWidth (1);
    }
  }
}

void curve_editor::mark_curve_segments () {
  for (int i = 0, j = curveinfo.size(); i < j; ++i) {
    curve_info& ci = curveinfo[i];
    if (ci.mark_segments) {
      multi_curve* crv = ci.curve;
      crvpt* pts = crv->get_points ();
      glColor3f (crv->r, crv->g, crv->b);
      for (int p = 0, q = crv->num_points (); p < q; ++p) {
        int dx, dy;	obj2win (pts[p].x, pts[p].y, dx, dy);
        glBegin (GL_LINES);
          glVertex2i (dx, dy);
          glVertex2i (dx, 0);
        glEnd ();
      }
    }
  }
}

void curve_editor::draw_handle (const point<float>& p) {
  int x, y; obj2win (p, x, y);
  float handle [] = {x - handle_radius, y, x, y + handle_radius, x + handle_radius, y, x, y - handle_radius};
  glBegin (GL_LINE_LOOP);
    glVertex2f (handle[0], handle[1]);
    glVertex2f (handle[2], handle[3]);
    glVertex2f (handle[4], handle[5]);
    glVertex2f (handle[6], handle[7]);
  glEnd ();
}

void curve_editor::draw_tangent (const point<float>& p, const point<float>& t) {
  int x, y; obj2win (p, x, y);
  int tx, ty; obj2win (t, tx, ty);
  glBegin (GL_LINES);
    glVertex2i (x, y);
    glVertex2i (tx, ty);
  glEnd ();

}

void curve_editor::draw_curve (multi_curve* crv) {
    
  crvpt* pts = crv->get_points ();  
  glBegin (GL_LINE_STRIP);
  for (int p = 0, q = crv->num_points (); p < q; ++p) {
    int dx, dy; obj2win (pts[p].x, pts[p].y, dx, dy);
    glVertex2i (dx, dy);
  }
  glEnd ();
  
}

void curve_editor::draw_vertices (multi_curve* crv) {
  
  const points_vector& vertices = crv->vertices;
  for (int p = 0, q = vertices.size(); p < q; ++p) {
    const point<float>& v = vertices[p];
    draw_handle (v);
  }

}

void curve_editor::draw_tangents (multi_curve* crv) {
  
  // draw tangents of ith curve

  const points_vector& vertices = crv->vertices;
  const points_vector& left_tangents = crv->left_tangents;
  const points_vector& right_tangents = crv->right_tangents;
  for (int p = 0, q = vertices.size(); p < q; ++p) {
    const point<float>& v = vertices[p];
    const point<float>& lt = left_tangents[p];
    const point<float>& rt = right_tangents[p];
    draw_tangent (v, lt);
    draw_tangent (v, rt);
    draw_handle (lt);
    draw_handle (rt);
  }
  
}

void curve_editor::dodo (list<undo_t>& do1, list<undo_t>& do2, string mesg) {
  if (do1.size() > 0) {
    undo_t& last = do1.back ();
		if (last.i < num_curves ()) {
			curve_info& ci = curveinfo[last.i];
			multi_curve* crv = ci.curve;
			do2.push_back (undo_t (last.i, *crv));
			*crv = last.curve;
			ci.lisner->edited (last.i);
			if (fft_enabled) dofft ();
			do1.pop_back ();
		}
		hitlist.clear ();
  } else cons << console::red << mesg << eol;
}

void curve_editor::add (multi_curve* crv, curve_listener* lsnr, bool disabled) {
  curveinfo.push_back (curve_info (crv, lsnr, disabled));
}

void curve_editor::clear () {
  curveinfo.clear ();
}

multi_curve* curve_editor::get_curve (int i) {
  if (i > -1 && i < (int) curveinfo.size()) {
    curve_info& ci = curveinfo [i];
    return ci.curve;
  }
  cout << "could not get curve " << i << eol;
  return 0;
}

int curve_editor::num_curves () {
  return curveinfo.size ();
}

curve_info& curve_editor::get_curve_info (int i) {
  return curveinfo[i];
}

void curve_editor::enter () {
  
	calc_win_mouse ();
  save_win_mouse ();
	if (fft_enabled) dofft ();
		
}

void curve_editor::clear_scratch_curve () {
  win_scratch_points.clear ();
  curv_scratch_points.clear ();
  scratch_curve.clear ();
  show_scratch_curve = false;
}

void curve_editor::attach_library (curve_library* lib) {
  library = lib;
}

void curve_editor::bg () {
  apply_mocap ();
}

void curve_editor::randomise () {
	
	// randomise a curve
	// useful for generating waveforms and L & R curves
	// assume vertices have ascending x.
	
	
	if (pik()); else pik = hit_t (0);
	multi_curve& crv = *curveinfo [pik.crv_id].curve;
	int nv = crv.num_vertices(), lst = nv - 1; 
	int ni = nv - 2; // num inserts
	if (nv < 1) return; 

	undos.push_back (undo_t (pik.crv_id, crv));
	
	point<float> first (crv.vertices[0].x, crv.vertices[0].y);
	point<float> last (crv.vertices[lst].x, crv.vertices[lst].y);
			
	crv.vertices.clear ();
	crv.left_tangents.clear ();
	crv.right_tangents.clear ();
	
	crv.add_vertex (first.x, first.y);
	crv.add_left_tangent (first.x, first.y);
	crv.add_right_tangent (first.x, first.y);
	
	rnd<float> rdy (rnd_miny, rnd_maxy);
	float x = 0;
	for (int i = 0; i < ni; ++i) {
		rnd<float>rdx (x, last.x);
		x = rdx ();
		float y = rdy();
		crv.add_vertex (x, y); 
		crv.add_left_tangent (x, y);
		crv.add_right_tangent (x, y);
	}
	
	crv.add_vertex (last.x, last.y);
	crv.add_left_tangent (last.x, last.y);
	crv.add_right_tangent (last.x, last.y);
	
	convert2_polyline (crv);
	
}

curve_info::curve_info  (multi_curve* c, curve_listener* l, bool dis) {
  curve = c;
  lisner = l;
  disabled = dis;
  mark_segments = false;
}

curve_info::curve_info () {
  curve = 0;
  lisner = 0;
  disabled = mark_segments = false;
}

hit_t::hit_t (multi_curve* cv, int cid, unsigned int w, int i)  {
	crv = cv;
  crv_id = cid;
  what = w;
  id = i;
}

void hit_t::clear () {
  crv = 0;
	crv_id = -1;
  what = NONE;
  id = -1;
}

multi_curve* hit_t::operator() () {
  return crv;
}

void curve_editor::dofft () {
	extern fft fft0;
	fft0.go (curveinfo[0].curve);
}

void curve_editor::clear_hit (hit_t& h) {
	h.clear ();
	use_existing_hit = false;
}
