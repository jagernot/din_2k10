#ifndef __checkdotdin
#define __checkdotdin

#include <stdlib.h>
#include <string>
using namespace std;

struct checkdotdin {
	
	checkdotdin () {
		
		extern string dotdin;
		dotdin = string(getenv ("HOME")) + string("/.din/");
		ifstream testf ((dotdin + "globals").c_str(), ios::in);
		if (testf) {
			cout << "using data files in " << dotdin << endl;
		} else {
			int ret = system ("checkdotdin");
			cout << ret << " copied initial data files into: " << dotdin << endl;
		}
		
	}
	
};

#endif
