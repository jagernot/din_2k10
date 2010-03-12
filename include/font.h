#ifndef _FONT
#define _FONT

#include <map>
#include <string>
#include <fstream>
#include <SDL/SDL_opengl.h>
#include <glyph.h>
using namespace std;

struct font {
   
   string fname;
   string name;
	 
   int nchars;
   int max_char_width, max_char_height;
   int charspc;
	 
   int cellsz;
   int xcells, ycells;
   
	 map <char, glyph> characters;
	 
	 map < char, map<char, int> > kern;
   
   void ncells ();
	 
	 bool modified;

   public:
   
    font();
    ~font ();
    font (const string& fname);
    void load (ifstream& file);
    void save ();
    void cell_size (int cs);
		
    int char_width (char c);
    int char_height (char c);
    
    void draw_char (char c, int x, int y, int z = 0);
    const string& filename () const { return fname;}
		
    const map<char, glyph>& get_chars ();
    void set_chars (const map<char, glyph>& chars);
		
};

extern font fnt;

inline font& get_font () {
	return fnt;
}

inline int get_line_height () {
  return fnt.max_char_height;
}

inline int max_char_width () {
  return fnt.max_char_width;
}

inline void draw_char (char c, int x, int y, int z = 0) {
  font& f = get_font ();
  f.draw_char (c, x, y, z);
}

int draw_string (const string& s, int x, int y, int z = 0);

#endif
