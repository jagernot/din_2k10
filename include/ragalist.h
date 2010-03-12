#ifndef __ragalist
#define __ragalist

#include <sstream>
#include <vector>
using namespace std;

struct raga {
	string name;
	string notes;
	int num_notes;
	raga () : num_notes(0) {}
	bool operator== (const raga& r) { return name == r.name;}
};

class ragalist {
	vector<raga> ragas;
	public:
		ragalist ();
		~ragalist ();
		void load ();
		void save ();
		void add (stringstream& ss);
		void remove (const string& name);
		bool get (const string& s, raga& r);
		void ls ();
};

#endif
