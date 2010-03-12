 #include "help.h"
 #include "console.h"
 #include <fstream>
 using namespace std;
 
extern console cons;
extern string dotdin;
 
 help::help (string fname) {
	 
   string s;
   ifstream file ((dotdin + fname).c_str(), ios::in);
	 if (!file) text.push_back (fname); else while (getline(file, s)) text.push_back (s);
	 
 }
 
 void help::operator() () {
	 
   for (int i = 0, j = text.size(); i < j; ++i) cons << console::cyan << text[i] << eol;
	 
 }
