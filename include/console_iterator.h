#ifndef __console_iterator
#define __console_iterator

#include <console.h>
#include <string>
using namespace std;

struct console_iterator {
  
  console& con;
  char delim;
  
  console_iterator (console& c, char delim = ' ');
  
  console_iterator& operator* ();
  console_iterator& operator++ ();
  console_iterator& operator++ (int);
  
  template <typename T> console_iterator& operator= (const T& t) {
    con << t;
    return *this;
  }
    
};

#endif
