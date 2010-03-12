#ifndef __STRENGTH_EDITOR
#define __STRENGTH_EDITOR

#include <curve_editor.h>

#include <map>
using namespace std;

struct note_marker {

  char note_name;
  int x;
  static int ybot, ytop;
  static int clearance;

};

struct din;

class strength_editor : public curve_editor {

  din* d;
  
  vector<note_marker> note_markers;

  int selected_resonator;

  help helptext;

  void build_note_markers ();
  void draw_note_markers ();
  
  public:

    strength_editor (din* dd, string settings_file, string help_file);
    
    bool handle_input ();
        
    void select_curve (int i);
    void draw_selected_resonator ();
    void draw ();
    
};

#endif
