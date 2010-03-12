#include <font_editor.h>
#include <fstream>
#include <font.h>
#include <input.h>
#include <console.h>
#include <iostream>
using namespace std;

extern console cons;

font_editor::font_editor (font& f, string settingsf, string helpf) : fn (f), glyphs (f.get_chars()), moving(false), helptext (helpf) {
  load_settings (settingsf);
  prep_edit ();
  edit_char (0);
}

font_editor::~font_editor () {
  leave ();
  save_settings ();
}

void font_editor::leave () {
  save_font ();
}

void font_editor::save_font () {
  save_char (ichar);
  prep_save ();
}

void font_editor::edit_char (int i) {
  if (i > -1 && i < (int) glyphs.size()) {
    char c = chars[i];
    gl = glyphs[c];
    lines = gl.lines;
		cout << "char = " << c << endl;
  }
}

void font_editor::save_char (int i) {
  if (i > -1 && i < (int)glyphs.size()) {
    gl.lines = lines;
    glyphs[chars[i]] = gl;
  }
}

void font_editor::load_settings (string fname) {

  // load settings from file
	extern string dotdin;
  ifstream file ((dotdin +fname).c_str (), ios::in);
  if (!file) return;
  settingsf = fname;
  basic_editor::load (file);
  
}

void font_editor::prep_edit () {
  chars.clear ();
  for (map<char, glyph>::iterator i = glyphs.begin (), j = glyphs.end(); i != j; ++i) {
    const pair<char, glyph>& p = *i;
    glyph g (p.second);
    vector<line>& lines = g.lines;
    for (int s = 0, t = lines.size(); s < t; ++s) {
      vector < point<int> >& points = lines[s].points;
      for (int m = 0, n = points.size(); m < n; ++m) {
        point<int>& pt = points[m];
        int wx, wy; 
        basic_editor::obj2win (pt.x / fnt.cellsz, pt.y / fnt.cellsz, wx, wy);
        pt.x = wx; 
        pt.y = wy;
      }
    }
    chars.push_back (p.first);
    glyphs[p.first] = g;
  }
}

void font_editor::prep_save () {
  map<char, glyph> fg (glyphs);
  for (map<char, glyph>::iterator i = fg.begin (), j = fg.end(); i != j; ++i) {
    const pair<char, glyph>& p = *i;
    glyph g(p.second);
    vector<line>& lines = g.lines;
    for (int s = 0, t = lines.size(); s < t; ++s) {
      vector < point<int> >& points = lines[s].points;
      for (int m = 0, n = points.size(); m < n; ++m) {
        point<int>& pt = points[m];
        float wx, wy; 
        basic_editor::win2obj (pt.x, pt.y, wx, wy);
        pt.x = wx * fnt.cellsz; 
        pt.y = wy * fnt.cellsz;
      }
    }
    g.find_width_height ();
    fg[p.first] = g;
		
  }
  fn.set_chars (fg);
}

void font_editor::save_settings () {

	extern string dotdin;
  ofstream file ((dotdin + settingsf).c_str(), ios::out);
  if (!file) return;
    
  basic_editor::save (file);

}

bool font_editor::handle_input () {
  
  basic_editor::handle_input ();
  
  if (keypressed ('f')) {
    float x, y; snap (x, y);
    scratch_points.push_back (point<float>(x, y));
  }
  
  if (keypressed ('g')) {
    if (scratch_points.size() > 1) lines.push_back (line(scratch_points));
    scratch_points.clear ();
  }
  
  if (keypressed ('r')) {
    if (moving) moving = false;
    else {
      cur = hittest ();
      if (cur()) moving = true;
    }
  } else if (moving) move ();
  
  if (keypressed ('i')) {
    if (ins()) {
      float x, y; snap (x, y);
      lines[ins.l].insert (ins.v, x, y);
      ins = line_hit();
    } else ins = hittest ();
  }
  
  if (keypressed ('v')) {
    del = hittest ();
    if (del()) {
      if (keydown (SDLK_LSHIFT)) 
        lines.erase (lines.begin() + del.l);
      else {
        if (lines[del.l].remove (del.v) < 2) lines.erase (lines.begin() + del.l);
      }
    } 
  }
  
  if (keypressedd ('[')) {
    if (ichar > 0) {
      save_char (ichar--);
      edit_char (ichar);
    }
  }
  
  if (keypressedd (']')) {
    if (ichar < ((int)chars.size() - 1)) {
      save_char (ichar++);
      edit_char (ichar);
    }
  }
  
  
  if (keypressed (SDLK_RETURN)) {
    save_font ();
  }
  
  
  
  return true;
}

void font_editor::draw () {
  
  project ();
    basic_editor::draw ();
    draw_lines ();
    draw_scratch_line ();
  unproject ();
  
}

line_hit font_editor::hittest () {
  float x, y; snap (x, y);
  for (int i = 0, j = lines.size(); i < j; ++i) {
    const line& l = lines[i];
    for (int p = 0, q = l.points.size(); p < q; ++p) {
      const point<int>& pt = l.points[p];
      if ((pt.x == x) && (pt.y == y)) return line_hit (i, p);
    }
  }
  return line_hit ();
}

void font_editor::draw_scratch_line () {
  if (int npts = scratch_points.size ()) {
    float x, y; snap (x, y);
    glColor3f (0.7, 0.4, 0);
    glBegin (GL_LINE_STRIP);
      for (int i = 0; i < npts; ++i) glVertex2f (scratch_points[i].x, scratch_points[i].y);
      glVertex2f (x, y);
    glEnd();
  }
}

void font_editor::draw_lines () {
  glColor3f (1, 0.8, 0);
  for (int i = 0, j = lines.size(); i < j; ++i) lines[i].draw ();
}

void font_editor::move () {
  float x, y;
  snap (x, y);
  lines[cur.l].set (cur.v, x, y);
}


