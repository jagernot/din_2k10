#ifndef __help
#define __help

#include <string>
#include <vector>
using namespace std;

struct help {
  vector<string> text;
  help (string fname);
  void operator() ();
};
#endif
