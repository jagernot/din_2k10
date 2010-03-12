#include "globals.h"
#include "din.h"
#include "command.h"
#include "console.h"
#include "math.h"
#include "delay.h"
#include "ragalist.h"
#include "font.h"
#include "glyph.h"
#include "chrono.h"
#include "utils.h"
#include "ui.h"
#include "curve_editor.h"
using namespace std;

extern console cons;

void cmdlist::add (command* cmd) {
	cmds.push_back (cmd);
}

command* cmdlist::run (const string& cmd, bool errmsg) {
		
	stringstream ss (cmd);
	string name; ss >> name;
	
	for (int i = 0, j = cmds.size(); i < j; ++i) {
		command* ci = cmds[i];
		if ((name == ci->longname) || (name == ci->shortname)) {
			(*ci)(ss);
			return ci;
		}
	}
	
	if (errmsg) cons << console::red << "error: " << cmd << " not found." << eol;
	return 0;
	
}

bool cmdlist::f1 (const string& cmd) {
	// help for one command
	if (cmd != "") {
		for (int i = 0, j = cmds.size(); i < j; ++i) {
			command* ci = cmds[i];
			if ((cmd == ci->longname) || (cmd == ci->shortname)) {
				ci->f1();
				return true;
			}
		}
	} else return false;
	
	cons << console::red << cmd << " not found." << eol;
	return false;
	
}

void cmdlist::list () {
	// list all available commands
	cons << console::yellow << "Commands available:" << eol;
	for (int i = 0, j = cmds.size(); i < j; ++i) 
		cons << tab << cmds[i]->longname << '/' << cmds[i]->shortname << eol;
	cons << console::yellow << "Type help <command> to find out more." << eol;
}

bool setv::operator() (stringstream& ss) {
	string varn; ss >> varn;
	if (varn == "tu" || varn == "tuning") {
		extern load_globals lg;
		string name; ss >> name;
		if (lg.load_intervals (name)) cons << "set tuning to: " << name << eol; else cons << console::red << "bad tuning file: " << name << eol;
		d->restore_all_notes ();
		return true;
	} else if (varn == "sc" || varn == "scroll") {
		int x, y;
		ss >> x >> y;
		d->dinfo.scroll = point<int>(x, y);
		return true;
	} else if (varn == "fmd" || varn == "fm_depth") {
		string s; ss >> s;
		d->fm_depth = atof (s.c_str());
		hz2step (d->fm_depth, d->fm_step);
		eresult = "success";
		return true;
	} else if (varn == "amd" || varn == "am_depth") {
		string s; ss >> s;
		d->am_depth = atof (s.c_str());
		eresult = "success";
		return true;
	} else if (varn == "ps1" || varn == "prompt") {
		ss >> cons.ps1;
		return true;
	}	else if (varn.length() == 1) {
		char c = varn[0]; 
		if (c == 'p') c = 'P';
		float fv; ss >> fv;
		d->retune_note (c, fv);
		return true;
	}	else {
		cons << console::red << "variable " << varn << " not found." << eol;
		return false;
	}
}

bool getv::operator() (stringstream& ss) {
	string varn; ss >> varn;
	cons << console::green;
	if (varn.length() == 1) {
		char c = varn[0];
		float fv; ss >> fv;
		cons << "note: " << c << " value: " << d->get_note_value (c) << eol;
		return true;
	} else if (varn == "sc" || varn == "scroll") {
		cons << varn << "x:" << d->dinfo.scroll.x << "y:" << d->dinfo.scroll.y << eol;
		return true;
	} else if (varn == "fmd" || varn == "fm_depth") {
		stringstream fss;
		fss << d->fm_depth;
		eresult = fss.str();
		cons << varn << ' ' << eresult << eol;
		return true;
	} else if (varn == "amd" || varn == "am_depth") {
		stringstream ass;
		ass << d->am_depth;
		eresult = ass.str ();
		cons << varn << ' ' << eresult << eol;
		return true;
	} else {
		cons << console::red << "variable " << varn << " not found." << eol;
		return false;
	}
}

bool listv::operator() (stringstream& ss) {
	cons << "tuning/tu - tuning of notes of raga. eg, sv tuning et|ji" << eol;
	cons << "s r R g G m M P d D n N S - each char is a var, eg, sv s 1, sv S 2, sv P 1.5 etc" << eol;
	cons << "scroll" << eol;
	return true;
}

bool show_help::operator() (stringstream& ss) {
	string cmd; ss >> cmd;
	if (!lst->f1(cmd)) lst->list ();
	return true;
}

bool set_delay::operator() (stringstream& ss) {
	
	vector<string> vs;
	
	string s, ls;
	while (1) {
		ss >> s;
		if (s == ls) break; else vs.push_back (s);
		ls = s;
	}
	
	if (vs.size() < 1) {
		eresult = "bad set_delay syntax";
		return false;
	}
	
	delay* d[2] = {left, right};
	delay** pd = d;
	int n = -1;
	
	s = vs[0];
	if (s == "left" || s == "l") n = 1;
	else if (s == "right" || s == "r") {
		pd++;
		n = 1;
	} else if (s == "all" || s == "a") n = 2;
	else {
		eresult = "bad delay name";
		return false;
	}
	
	for (int i = 0; i < n; ++i, pd++) {
		
		float t, f, v;
		(*pd)->get (t, f, v);
		
		for (int j = 1, k = vs.size() - 1; j < k; j += 2) {
			string pj = vs[j], vj = vs[j+1];
			if (pj == "time" || pj == "t") t = atof (vj.c_str());
			else if (pj == "feedback" || pj == "f") f = atof (vj.c_str());
			else if (pj == "volume" || pj == "v") v = atof (vj.c_str());
			else if (pj == "all" || pj == "a") {
				t = atof (vj.c_str());
				f = atof (vj.c_str());
				v = atof (vj.c_str());
			}
		}
		
		if (t <= 0) t = 1;
		clip<float> (0, f, 1);
		clip<float> (0, v, 1);
		
		(*pd)->set (t, f, v);
		
	}
		
	eresult = "success";
	return true;
	
}

bool get_delay::operator() (stringstream& ss) {
	
	delay* d;
	string s; ss >> s;
	if (s == "left" || s == "l") d = left; 
	else if (s == "right" || s == "r") d = right; 
	else return false;
	
	float t, f, v;
	d->get (t, f, v);
	
	char buf[1024];
	sprintf (buf, "delay = %s, time = %0.2f, feedback = %0.2f, volume = %0.2f", s.c_str(), t, f, v);

	cons << console::cyan << buf << eol;
	
	eresult = buf;
	return true;
	
}

bool set_bpm::operator() (stringstream& ss) {
	
	string s; ss >> s;
	string v; ss >> v;
	float bpm = atof (v.c_str());
	if (bpm < 0) bpm = 0;
	beat2value* bv [] = { &d->gatl, &d->gatr, &d->am, &d->fm };
	string str[] = { "gate-l", "gate-r", "am", "fm" };
	int set [] = {0, 0, 0, 0};

	if (s == "gate") set[0]=set[1]=1; else if (s == "all") set[0]=set[1]=set[2]=set[3]=1; else {
		for (int i = 0; i < 4; ++i) {
			if (s == str[i]) {
				set[i] = 1;
				break;
			}
		}
	}

	eresult = "bad bpm";
	bool result = false;
	
	char buf[256];
	for (int i = 0; i < 4; ++i) {
		if (set[i]) {
			bv[i]->set_bpm (bpm);
			//cons << str[i] << " bpm = " << bpm << eol;
			sprintf (buf, "%f", bpm);
			eresult = buf;
			result = true;
		}
	}
	
	return result;
}

bool get_bpm::operator() (stringstream& ss) {
	string s; ss >> s;
	string ch [] = {"gate-l", "gate-r", "am", "fm"};
	beat2value* g [] = { &d->gatl, &d->gatr, &d->am, &d->fm };
	char buf[256];
	for (int i = 0; i < 4; ++i) {
		if (s == ch[i]) {
			sprintf (buf, "%.3f", g[i]->get_bpm());
			cons << buf << eol;
			eresult = buf;
			return true;
		}
	}
	eresult = "cant find bpm for " + s;
	return false;
}

bool add_raga::operator() (stringstream& ss) {
	extern ragalist ragalst;
	ragalst.add (ss);
	return true;
}

bool ls_raga::operator() (stringstream& ss) {
	extern ragalist ragalst;
	ragalst.ls ();
	return true;
}

bool remove_raga::operator() (stringstream& ss) {
	string nam; ss >> nam;
	if (nam != "") {
		extern ragalist ragalst;
		ragalst.remove (nam);
		return true;
	} 
	return false;
}

bool load__raga::operator() (stringstream& ss) {
	string nam; ss >> nam;
	if (nam != "") {
		extern ragalist ragalst;
		raga r;
		if (ragalst.get (nam, r)) {
			d->save_raga ();
			d->ragainfo.set_raga (nam);
			d->load_raga ();
			d->enter ();
			return true;
		} else {
			cons << console::red << "raga: " << nam << " not found." << eol;
			return false;
		}
	} else {
		cons << console::red << "bad raga name" << eol;
		return false;
	}
}

bool set_kern::operator() (stringstream& ss) {
	
	extern font fnt;
	
	char a, b;
	int k;
	ss >> a >> b >> k;
	
	if (a == '.') {
		map<char, glyph>& m = fnt.characters;
		for (map<char, glyph>::iterator i = m.begin(), j = m.end(); i != j; ++i) fnt.kern[(*i).first][b] = k;
		fnt.modified = true;
	} else if (b == '.') {
		map<char, glyph>& m = fnt.characters;
		for (map<char, glyph>::iterator i = m.begin(), j = m.end(); i != j; ++i) fnt.kern[a][(*i).first] = k;
		fnt.modified = true;
	} else {
		fnt.kern[a][b] = k;
		fnt.modified = true;
	}
	
	return true;
	
}

bool get_kern::operator() (stringstream& ss) {
	
	extern font fnt;
	
	char a, b;
	int k;
	ss >> a >> b >> k;
	
	if (a == '.') {
		map<char, glyph>& m = fnt.characters;
		for (map<char, glyph>::iterator i = m.begin(), j = m.end(); i != j; ++i) {
			a = (*i).first;
			cons << a << ' ' << b << ' ' << fnt.kern[a][b] << eol;
		}
	} else if (b == '.') {
		map<char, glyph>& m = fnt.characters;
		for (map<char, glyph>::iterator i = m.begin(), j = m.end(); i != j; ++i) {
			b = (*i).first;
			cons << a << ' ' << b << ' ' << fnt.kern[a][b] << eol;
		}
	} else {
		cons << a << ' ' << b << ' ' << fnt.kern[a][b] << eol;
	}
	
	return true;
	
}

bool set_font_size::operator() (stringstream& ss) {
		
	extern font fnt;
	
	int cellsz, charspc;
	ss >> cellsz >> charspc;
	if (cellsz > 0) fnt.cell_size (cellsz);
	if (charspc > 0) fnt.charspc = charspc * cellsz;
	fnt.modified = true;
	
	extern console cons;
	cons.calc_lines_per_screen ();
		
	return true;
	
}

bool load__resonators ::operator() (stringstream& ss) {
	string fname; ss >> fname;
	if (fname == ".") d->load_resonators (d->dinfo.resonators_fname); else d->load_resonators (fname);
	return true;
}

bool save__resonators ::operator() (stringstream& ss) {
	string fname; ss >> fname;
	if (fname != ".") d->dinfo.resonators_fname = fname;
	int deldis = 0; ss >> deldis;
	d->save_resonators (deldis);
	return true;
}

bool delete_disabled_resonators ::operator() (stringstream& ss) {
	extern cmdlist cmdlst;
	cmdlst.run ("sre . 1");
	cmdlst.run ("lre .");
	return true;
}

const char* western1 [] = {
	"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C"
};

const char* western2 [] = {
	"C", "Db", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B", "C"
};

const char* indian [] = {
	"s", "r", "R", "g", "G", "m", "M", "P", "d", "D", "n", "N", "S"
};

const int NUM_NOTES = 13;

const float frequencies [] = { // from wikipedia - piano key frequencies page.
	261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440, 466.164, 493.883, 523.251
};

int get_note_num (const string& s) {
	for (int i = 0; i < NUM_NOTES; ++i) {
		if (s == western2[i] || s == western1[i]) return i;
	}
	cons << console::red << "bad note: " << s << eol;
	return -1;
}

bool note_distance::operator() (stringstream& ss) {
	
	string note1, note2;
	ss >> note1 >> note2;
	if (note1.length() && note2.length()) {
		note1[0] = toupper (note1[0]);
		note2[0] = toupper (note2[0]);
	} else return false;
	
	static const char* dist [] = {
		"same note", "minor second", "second", "minor third", "third", "perfect fourth",
		"diminished fifth", "perfect fifth", "minor sixth", "sixth", "minor seventh",
		"seventh", "octave"
	};
	
	int i = get_note_num (note1), j = get_note_num (note2);
	if (i == -1 || j == -1) return false; else {
		if (j <= i) j += 12;
		int ji = j - i;
		cons << dist[ji] << eol;
	}
	
	return true;
	
}

bool chord::operator() (stringstream& ss) {
	
	string root, type;
	ss >> root >> type;
	
	if (root.length()) root[0] = toupper (root[0]); else return false;
	int nn = get_note_num (root);
	if (nn == -1) return false; else {
		
		const int ntypes = 20;
		static const char* types[] = {
			"", // major
			"m", // minor
			"maj7", // major 7
			"m7", // minor 7
			"sus4", // suspended 4th
			"7sus4", // dominant 7th, suspended 4th
			"6", // major 6th
			"m6", // minor 6th
			"7", // dominant 7th
			"9", // dominant 9th
			"5", // power chord
			"69", // maj 6th, add 9
			"11", // dominant 11th
			"13", // dominant 13th
			"add9", // major add 9th
			"m9", // minor 9th
			"maj9", // major 9th
			"+", // augmented
			"07", // diminished 7th
			"0" // diminished triad
		};
		
		static const int steps[][7] = {
			{3, 0, 4, 7}, // num_notes, root, note1, note2....
			{3, 0, 3, 7},
			{4, 0, 4, 7, 11},
			{4, 0, 3, 7, 10},
			{3, 0, 5, 7},
			{4, 0, 5, 7, 10},
			{4, 0, 4, 7, 9},
			{4, 0, 3, 7, 9},
			{4, 0, 4, 7, 10},
			{5, 0, 4, 7, 10, 14},
			{2, 0, 7},
			{5, 0, 4, 7, 9, 14},
			{6, 0, 4, 7, 10, 14, 17},
			{6, 0, 4, 7, 10, 14, 21},
			{4, 0, 4, 7, 14},
			{5, 0, 3, 7, 10, 14},
			{5, 0, 4, 7, 11, 14},
			{3, 0, 4, 8},
			{4, 0, 3, 6, 10},
			{3, 0, 3, 6}
		};
		
		int t = -1;
		for (int i = 0; i < ntypes; ++i) {
			if (type == types[i]) {
				t = i; break;
			}
		}
		
		if (t == -1) return false; else {
			const int* ch = &steps[t][0];
			stringstream ss1, ss2;
			for (int i = 0, j = ch[0], k = 1; i < j; ++i, ++k) {
				int p = (nn + ch[k]) % 12;
				ss1 << western1[p] << ' ';
				ss2 << western2[p] << ' ';
			}
			cons << ss2.str() << " or " << ss1.str() << eol;
			return true;
		}
		
		
	}
}

void sa::find_nearest_note (string& note, float& frequency, float& dist) {
	
	float left = frequencies[0] / 16, right = 2 * left;
	float saa = d->get_sa ();
	if (saa < left) return;
	
	while (1) {
		if ((left <= saa) && (right > saa)) break;
		else { 
			left*=2; 
			right*=2;
		}
	}
	
	float oct = left / frequencies[0];
	dist = saa - left;
	int key = 0; 
	float tone = 0;
	for (int i = 0; i < 13; ++i) {
		tone = frequencies[i] * oct;
		float newdist = tone - saa; if (newdist < 0) newdist = -newdist;
		if (newdist < dist) {
			dist = newdist;
			key = i;
		}
	}	
	
	string nn;
	if (western2[key] == western1[key]) nn = western1[key]; else nn = (string)western2[key] + "/" + (string)western1[key];
	note = nn;
	frequency = frequencies[key] * oct;
	dist = saa - frequency;
	d->set_key (key);

}

bool sa::operator() (stringstream& ss) {

	string s; ss >> s;
	if (s == "") {
		// print sa
		string note; 
		float frequency, dist;
		find_nearest_note (note, frequency, dist);
		
		char buf [512];
		sprintf (buf, "sa/key is %0.3f Hz, nearest note is %s/%0.3f Hz, distance is %0.3f Hz", d->get_sa(), note.c_str(), frequency, dist);
		cons << console::cyan << buf << eol;
		
		eresult = buf;
		return true;

	} else {
		
		float f = d->get_sa ();
		int k = d->get_key ();
		int o = 0;
		float v = atof (s.c_str());
		if (v <= 0) { // a note
			if (s[0] != '.') {
				s[0] = toupper(s[0]);
				for (int i = 0; i < 12; ++i) {
					if ((s == western1[i]) || (s == western2[i])) {
						f = frequencies[i];
						k = i;
						break;
					}
				}
			}
			
			// octave
			ss >> s;
			o = atoi (s.c_str());
			
			f = f * pow (2, o);
			bool saok = check_sa (f);
			if (saok) {
				d->set_sa (f);
				d->set_key (k);
				eresult = "success";
				return true;
			}
			
		} else {
			
			f = v;
			bool saok = check_sa (f);
			if (saok) {
				d->set_sa (f);
				string note; float frequency, dist; 
				find_nearest_note (note, frequency, dist);
				eresult = "success";
				return true;
			}
			
		}
		
		eresult = "failure";
		return false;
				
	}
}

bool sa::check_sa (float f) {
	const float min_sa = frequencies[0] / 8, max_sa = 20000;
	if (f < min_sa) {
		eresult = "sa/key too low";
		cons << console::red << eresult << eol;
		return false;
	} else if (f > max_sa) {
		eresult = "sa/key too high";
		cons << console::red << eresult << eol;
		return false;
	} else return true;
}

bool notation::operator () (stringstream& ss) {
	
	string s;
	ss >> s;
	
	if (s == "i" || s == "ind" || s == "indian") d->set_notation ("indian"); else
	if (s == "w" || s == "west" || s == "western") d->set_notation ("western");
	
	return true;
	
}

bool echo::operator() (stringstream& ss) {
	string s, ls;
	string spew;
	while (1) {
		ss >> s;
		if (s == ls) break;
		spew = spew + s + ' ';
		ls = s;
	} 
	cons << console::cyan << spew << eol;
	return true;
}

bool set_pitch_and_volume::operator() (stringstream& ss) {
	
	float pitch, volume;
	string ignore;
	
	ss >> pitch >> volume;
	
	extern int mousex, mousey;
	extern viewport view;
	
	mousex = pitch * view.xmax;
	mousey = volume * view.ymax;
	
	cons << pitch << ' ' << volume << eol;
		
	return true;
	
}

bool curve_name::operator() (stringstream& ss) {
	extern uis_t uis;
	curve_editor* ced = uis.current_curve_editor;
	if (ced && ced->pik()) {
		multi_curve* crv = ced->curveinfo[ced->pik.crv_id].curve;
		ss >> crv->name;
	}
	return true;
}
