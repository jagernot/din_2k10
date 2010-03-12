#ifndef __command
#define __command

#include <vector>
#include <sstream>
#include <string>
#include "help.h"
using namespace std;

struct command {
	
	string longname, shortname;
	help f1;
	string eresult;
	
	command (const string& ln, const string& sn, const string& fname) : longname (ln), shortname (sn), f1(fname) {}
	bool operator== (const command& c) {
		return ((c.longname == longname) || (c.shortname == shortname));
	}
	virtual bool operator() (stringstream& ss) { return true; }
	virtual ~command () {}
	
};

struct cmdlist {
	vector<command*> cmds;
	public:
		void add (command* cmd);
		command* run (const string& cmd, bool errmsg = true);
		bool f1 (const string& cmd);
		void list ();
};

struct din;

struct setv : command { // set var
	din* d;
	public:
		setv (din* dd, const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp), d(dd) {}
		bool operator() (stringstream& ss);
		~setv () {}
};

struct getv : command { // get var
	din* d;
	public:
		getv (din* dd, const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp), d(dd) {}
		bool operator() (stringstream& ss);
		~getv () {}
};

struct listv : command { // list vars
	public:
		listv (const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp) {}
		bool operator() (stringstream& ss);
		~listv () {}
};

struct delay;

struct set_delay: command {
	delay* left;
	delay* right;
	public:
		set_delay (delay* l, delay* r, const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp), left(l), right(r) {}
		bool operator() (stringstream& ss);
		~set_delay () {}
};

struct get_delay: command {
	delay* left;
	delay* right;
	public:
		get_delay (delay* l, delay* r, const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp), left(l), right(r) {}
		bool operator() (stringstream& ss);
		~get_delay () {}
};

struct set_bpm: command {
	din* d;
	public:
		set_bpm (din* dd, const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp), d(dd) {}
		bool operator() (stringstream& ss);
		~set_bpm () {}
};

struct get_bpm: command {
	din* d;
	public:
		get_bpm (din* dd, const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp), d(dd) {}
		bool operator() (stringstream& ss);
		~get_bpm () {}
};

struct add_raga: command {
	public:
		add_raga (const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp) {}
		bool operator () (stringstream& ss);
		~add_raga () {}
};

struct ls_raga: command {
	public:
		ls_raga (const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp) {}
		bool operator () (stringstream& ss);
		~ls_raga () {}
};

struct remove_raga : command {
	public:
		remove_raga (const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp) {}
		bool operator () (stringstream& ss);
		~remove_raga () {}
};

struct load__raga : command {
	din* d;
	public:
		load__raga (din* dd, const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp), d(dd) {}
		bool operator () (stringstream& ss);
		~load__raga () {}
};

struct show_help : command {
	cmdlist* lst;
	public:
		show_help (const string& ln, const string& sn, const string& hlp, cmdlist* lt) : command (ln, sn, hlp), lst (lt) {}
		bool operator() (stringstream& ss);
		~show_help () {}
};

struct sa : command {
	din* d;
	public:
		sa (din* dd, const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp), d(dd) {}
		bool operator() (stringstream& ss);
		~sa () {}
		void find_nearest_note (string& note, float& frequency, float& dist);
		bool check_sa (float f);
};

struct note_distance: command {
	public:
		note_distance (const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp) {}
		bool operator() (stringstream& ss);
		~note_distance () {}
};

struct chord: command {
	public:
		chord (const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp) {}
		bool operator() (stringstream& ss);
		~chord () {}
};

struct set_kern: command {
	
	public:
		set_kern (const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp) {}
		bool operator() (stringstream& ss);
		~set_kern () {}
	
};

struct get_kern: command {
	
	public:
		get_kern (const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp) {}
		bool operator() (stringstream& ss);
		~get_kern () {}
	
};

struct set_font_size: command {
	
	public:
		set_font_size (const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp) {}
		bool operator() (stringstream& ss);
		~set_font_size () {}
	
};

struct load__resonators : command {
	din* d;
	load__resonators (din* dd, const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp), d(dd) {}
	bool operator() (stringstream& ss);
	~load__resonators () {}
};

struct save__resonators : command {
	din* d;
	save__resonators (din* dd, const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp), d(dd) {}
	bool operator() (stringstream& ss);
	~save__resonators () {}
};

struct delete_disabled_resonators : command {
	din* d;
	delete_disabled_resonators (din* dd, const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp), d(dd) {}
	bool operator() (stringstream& ss);
	~delete_disabled_resonators () {}
};


struct set_depth : command {
};

struct din_info;

struct notation : command {

	din* d;
	notation (din* dd, const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp), d(dd) {}
	bool operator() (stringstream& ss);
	~notation () {}

};

struct echo : command {
	echo (const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp) {}
	bool operator() (stringstream& ss);
	~echo () {}
};

struct set_pitch_and_volume : command {
	
	din* d;
	
	set_pitch_and_volume (din* dd, const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp), d(dd) {}
	bool operator () (stringstream& ss);
	~set_pitch_and_volume () {}
	
};

struct curve_name : command {
	curve_name (const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp) {}
	bool operator() (stringstream& ss);
	~curve_name () {}
};


#endif
