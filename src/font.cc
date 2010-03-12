#include <font.h>
#include <iostream>
#include <sstream>
using namespace std;

font::font (const string& filename) : modified (false) {
	
	fname = filename;
	extern string dotdin;
  ifstream file ((dotdin + fname).c_str(), ios::in);
  if (!file) {
  	cout << "bad font file: " << fname << endl; 
  } else {
  	load (file);
  	cell_size (cellsz);
  	cout << "loaded font from: " << fname << endl;
  }
  
}

font::~font () {
  if (modified) save ();
}

void font::load (ifstream& file) {

  string ignore;

  file >> ignore >> name;
  file >> ignore >> nchars;

	file >> ignore >> cellsz;
	file >> ignore >> charspc;
  
  for (int i = 0; i < nchars; i++) {
      char c; file >> c;
      glyph g; 
      int nln; file >> nln;
      for (int l = 0; l < nln; ++l) {
        line ln;
        int npts; file >> npts;
        for (int s = 0; s < npts; ++s) {
          float x, y; file >> x >> y;
          ln.points.push_back (point<int>(x * cellsz, y * cellsz));
        }
        g.lines.push_back (ln);
      }
			g.find_width_height ();
      characters[c] = g;
  }
  
	int nkerns = 0;
	file >> ignore >> nkerns;
	for (int i = 0; i < nkerns; ++i) {
		char a, b;
		int k;
		file >> a >> b >> k;
		kern [a][b] = k * cellsz;
	}
	
	
      
}

void font::save () {
	
  extern string dotdin;
  ofstream file ((dotdin + fname).c_str(), ios::out);
  if (!file) {
  	cout << "cannot save font in: " << fname << endl;
  	return;
	} else {
		cout << "saving font in: " << fname << endl;
	}
  
  file << "name " << name << endl;
  file << "num_chars " << nchars << endl;
  
	file << "cell_size " << cellsz << endl;
	file << "char_spc " << charspc << endl;
	
  for (map<char, glyph>::iterator i = characters.begin(), j = characters.end(); i != j; ++i) {
    const pair<char, glyph>& p = *i;
		file << p.first << ' ';
		glyph g(p.second);
		vector<line>& lines = g.lines;
		int nlines = lines.size ();
		file << nlines << ' ';
		for (int s = 0; s < nlines; ++s) {
			vector < point<int> >& points = lines[s].points;
			int npts = points.size ();
			file << npts << ' ';
			for (int m = 0; m < npts; ++m) {
				point<int>& pt = points[m];
				file << pt.x/cellsz << ' ' << pt.y/cellsz << ' ';
			}
		}
		file << endl;
  }
	
	stringstream ss;
	int nk = 0;
	for (map<char, map<char, int> >::iterator i = kern.begin(), j = kern.end(); i != j; ++i) {
		char a = (*i).first;
		map<char, int>& m = (*i).second;
		for (map<char, int>::iterator p = m.begin(), q = m.end(); p != q; ++p) {
			char b = (*p).first;
			int k = (*p).second;
			if (k != 0) {
				ss << a << ' ' << b << ' ' << k/cellsz << '\n';
				++nk;
			}
		}
	}
	
	file << "num_kerns " << nk << endl;
	file << ss.str();
		      
}

int font::char_width (char c) { return characters[c].width; }
int font::char_height (char c) { return characters[c].height; }

void font::draw_char (char c, int x, int y, int z) {
  
  const glyph& gl = characters[c];
  const vector<line>& lines = gl.lines;
  
  for (int i = 0, nlines = lines.size(); i < nlines; ++i) {
    const line& li = lines[i];
    const vector< point<int> >& points = li.points;
    int npts = (int) points.size();
    if (npts) {
      glBegin (GL_LINE_STRIP);
        for (int j = 0; j < npts; ++j) {
          const point<int>& p = points[j]; 
          glVertex3i(x + (int) p.x, y + (int) p.y, z); // * cellsz, y + (int) p.y * cellsz, z);
        }
      glEnd ();
      
    }

  }
}

void font::cell_size (int sz) {
	
	float mul = sz * 1./cellsz;
	
	max_char_width = max_char_height = 0;
	
  for (map<char, glyph>::iterator i = characters.begin(), j = characters.end(); i != j; ++i) {
    pair<const char, glyph>& p = *i;
    glyph& g = p.second;
		vector<line>& lines = g.lines;
		int nlines = lines.size ();
		for (int s = 0; s < nlines; ++s) {
			vector < point<int> >& points = lines[s].points;
			int npts = points.size ();
			for (int m = 0; m < npts; ++m) {
				point<int>& pt = points[m];
				pt.x *= mul;
				pt.y *= mul;
			}
		}
		g.find_width_height ();
    if (g.width > max_char_width) max_char_width = g.width;
    if (g.height > max_char_height) max_char_height = g.height;
  }
	
	static const int headroom = 2;
	max_char_height += headroom;

	for (map<char, map<char, int> >::iterator i = kern.begin(), j = kern.end(); i != j; ++i) {
		char a = (*i).first;
		map<char, int>& m = (*i).second;
		for (map<char, int>::iterator p = m.begin(), q = m.end(); p != q; ++p) {
			char b = (*p).first;
			int k = (*p).second;
			if (k != 0) {
				kern[a][b] = k * mul;
			}
		}
	}
	
  cellsz = sz;
	
}

const map<char, glyph>& font::get_chars () {
  return characters;
}

void font::set_chars (const map<char, glyph>& chars) {
  characters = chars;
	modified = true;
}

int draw_string (const string& s, int x, int y, int z) {
	font& f = get_font ();
	int spc = f.charspc;
	char prevc = ' ';
  for (int p = 0, q = s.length(); p < q; ++p) {
      char c = s[p];
			if (c == ' ') x += 2 * spc; else {
				x += f.kern[prevc][c];
				f.draw_char (c, x, y, z);
				x = x + f.char_width (c) + spc;
			}
			prevc = c;
  }
  return x;
}
