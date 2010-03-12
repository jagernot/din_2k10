#ifndef __CURVE_LIBRARY
#define __CURVE_LIBRARY

#include <string>
#include <vector>
#include <multi_curve.h>
using namespace std;

class curve_editor;
class curve_library;

class curve_library {
  
  // a library of multi curves loaded from file
  
  string filename;
  
  vector <multi_curve> curves;
  int cur;
          
  void load (const string& fname);
  
  
  public:
  
    curve_library ();
    curve_library (const string& fname);
    ~curve_library ();
    void add (const multi_curve& m);
    void del ();
    void replace (const multi_curve& m);
    void insert (const multi_curve& m);
    const multi_curve& prev ();
    const multi_curve& next ();
    void move (int i);
    int num_curves ();
    void save ();
  
};
#endif
