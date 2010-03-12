#include <strength_editor.h>
#include <din.h>
#include <input.h>
#include <font.h>

bool strength_editor::handle_input () {

  curve_editor::handle_input ();

  if (keypressed (SDLK_F1)) helptext();
  if (keydown (SDLK_LCTRL) && keypressed ('r')) d->add_resonator ();
    
  return true;

}

void strength_editor::build_note_markers () {

  extern int NUM_INTERVALS;
  extern map<char, float> INTERVALS;

  int dx, dy;
  point<float> v;
  obj2win (v, dx, note_marker::ybot);
  v.y = 1; obj2win (v, dx, note_marker::ytop);
  note_marker::clearance = get_line_height ();
  
	
  for (int i = 0, noctaves = 3; i < noctaves; ++i) {
    map <char, float>::iterator iter = INTERVALS.begin ();
    for (int j = 0; j < NUM_INTERVALS; ++j, ++iter) {
      const pair <char, float>& p = *iter;
      note_marker nm;
      nm.note_name = p.first;
      extern char FIRST_INTERVAL_NAME, LAST_INTERVAL_NAME;
      if ((nm.note_name == FIRST_INTERVAL_NAME) || (nm.note_name == LAST_INTERVAL_NAME)) nm.note_name = LAST_INTERVAL_NAME;
      float m = i + p.second - 1;
      v.x = m; v.y = 0; obj2win (v, nm.x, dy);
      note_markers.push_back (nm);
    }
  }


}

void strength_editor::draw_note_markers () {

  glColor3f (0.6, 0.6, 1.);
  
  int dyb = note_marker::ybot - note_marker::clearance;
  int dyt = note_marker::ytop + note_marker::clearance;
  
  for (int i = 0, j = note_markers.size(); i < j; ++i) {
    const note_marker& nm = note_markers[i];
    glEnable (GL_LINE_STIPPLE);
    glLineStipple (1, 0x001F);
    glBegin (GL_LINES);
      glVertex2f (nm.x, note_marker::ybot);
      glVertex2f (nm.x, note_marker::ytop);
    glEnd ();
    glDisable (GL_LINE_STIPPLE);
    draw_char (nm.note_name, nm.x, dyb);
    draw_char (nm.note_name, nm.x, dyt);
  }

}

strength_editor::strength_editor (din* dd, string settings_file, string help_file) : curve_editor (settings_file), d (dd), helptext (help_file)  {

  build_note_markers ();
  selected_resonator = -1;

}

void strength_editor::select_curve (int i) {
  selected_resonator = i;
}

void strength_editor::draw_selected_resonator () {
  if (selected_resonator > -1) {
    multi_curve* crv = get_curve (selected_resonator);
    glColor3f (0, 1, 0);
    draw_curve (crv);
  }
}

void strength_editor::draw () {
  
  project ();
  
    draw_common ();
    draw_all ();
    draw_selected_resonator ();
    draw_note_markers ();
    
  unproject ();
}

int note_marker::ybot = 0, note_marker::ytop = 0;
int note_marker::clearance = 0;
  
