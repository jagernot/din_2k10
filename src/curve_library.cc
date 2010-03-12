#include <curve_library.h>
#include <curve_editor.h>
#include <console.h>
#include <utils.h>

extern console cons;

void curve_library::load (const string& fname) {
  
	extern string dotdin;
  ifstream inf ((dotdin + fname).c_str(), ios::in);
  
  if (inf) {
    
    string ignore;
    
    cout << "loading curve library: " << fname << endl;

    int ncurves; inf >> ignore >> ncurves;
    cout << "num curves: " << ncurves << eol;
    for (int i = 0; i < ncurves; ++i) {
      multi_curve m; 
      m.load (inf);
      curves.push_back (m);
    }
    
    cout << "curve library loaded." << endl;
  } 
  
  cur = 0;
}

curve_library::curve_library () {
  filename = " ";
  cur = 0;
}

curve_library::curve_library (const string& fname) {
  filename = fname;
  load (filename);
}

curve_library::~curve_library () {
  save ();
}

void curve_library::add (const multi_curve& m) {
  curves.push_back (m);
  cons << "added curve." << eol;
}

void curve_library::del () {
  curves.erase (curves.begin() + cur);
  clip<int> (0, cur, curves.size () - 1);
  cons << "erased curve: " << cur << eol;
}

void curve_library::replace (const multi_curve& m) {
  curves[cur] = m;
}

void curve_library::insert (const multi_curve& m) {
  curves.insert (curves.begin()+ cur, m);
  cons << "inserted curve at: " << cur << eol;
}

int curve_library::num_curves () {
  return curves.size ();
}

const multi_curve& curve_library::prev () {
  if (--cur < 0) cur = curves.size() - 1;
  cons << "curve " << cur << " loaded." << eol;
  return curves[cur];
}

const multi_curve& curve_library::next () {
  if (++cur >= (int)curves.size()) cur = 0;
  cons << "curve " << cur << " loaded." << eol;
  return curves[cur];
}

void curve_library::move (int i) {
  
  multi_curve& from = curves[cur];
  
  cons << "move from: " << cur;
  
  int j = cur + i;
  int last = curves.size() - 1;
  if (j < 0 || j > last) return; 
  
  multi_curve to = curves[j];
  curves[j] = from;
  from = to;
  cur = j;
  
  cons << " to: " << cur << eol;
  
}

void curve_library::save () {
	extern string dotdin;
  ofstream outf ((dotdin + filename).c_str(), ios::out);
  if (outf) {
    outf << "num_curves " << num_curves () << endl;
    for (int i = 0, j = num_curves(); i < j; ++i) {
      curves[i].save (outf);
    }
  }
}
