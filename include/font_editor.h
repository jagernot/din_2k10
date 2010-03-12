#ifndef __font_editor
#define __font_editor

#include <viewwin.h>
#include <ui.h>
#include <point.h>
#include <line.h>
#include <map>
#include <help.h>
#include <basic_editor.h>
#include <glyph.h>
#include <font.h>
using namespace std;

struct line_hit {
  int l; // line?
  int v; // vertex?
  line_hit () {
    l = v = -1; // invalid
  }
  line_hit (int ll, int vv) : l(ll), v(vv) {}
  bool operator() () {
    return ( (l > -1) && (v > -1));
  }
};

class font_editor : private basic_editor, public ui {
  
  font& fn;
  
  string settingsf;
  
  int nchars;
  string name;
  
  map<char, glyph> glyphs;
  vector<char> chars;
  glyph gl;
  vector<line> lines;
  int ichar;
  
  vector< point<float> > scratch_points;
  
  line_hit cur, ins, del;
  bool moving;

  help helptext;
    	  
  void prep_edit ();
  void prep_save ();
  void edit_char (int i);
  void save_char (int i);

	public:

		font_editor (font& f, string settingsf, string helpf = "../data/font_editor_help.txt");
		virtual ~font_editor ();
				
		void load_settings (string fname);
		void save_font ();
		void save_settings ();
		

		bool handle_input ();
		void draw ();
		void draw_lines ();
		void draw_scratch_line ();
		
		line_hit hittest ();
		
		void move ();
		
		void leave ();



};
#endif
