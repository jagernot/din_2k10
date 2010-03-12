#ifndef __osc
#define __osc

#include <lo/lo.h>
#include "chrono.h"
#include "command.h"
#include <string>
using namespace std;

struct oid : command {
	
	string name;
	
	oid (const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp) {}
	bool operator() (stringstream& ss);
		
};

struct oserver : command {
	
	lo_server server;
	int lo_fd;
	fd_set rfds;
	struct timeval tv;
	bool enabled;
	bool msg;
	
	oserver (const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp) {}
	bool operator() (stringstream& ss);
	void handle_requests ();
	void toggle ();
	~oserver ();
	
};

int command_handler (const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);

struct oconnect : command {
	
	lo_address& client;
	
	oconnect (lo_address& c, const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp), client(c) {}
	bool operator() (stringstream& ss);
	~oconnect () {}
	
};

struct osend : command {
	
	lo_address& client;
	
	osend (lo_address& c, const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp), client(c) {}
	bool operator() (stringstream& ss);
	~osend () {}
	
};

struct otransmit : command {
	
	lo_address& client;
	otransmit (lo_address& c, const string& ln, const string& sn, const string& hlp) : command (ln, sn, hlp), client(c) {}
	bool operator() (stringstream& ss);
	~otransmit() {}
		
};

struct osc_out {
	lo_address& client;
	string path;
	string format;
	osc_out (lo_address& c, const string& p, const string& f) : client(c), path(p), format(f) {}
};

struct sender {
	virtual bool operator()() = 0;
	virtual ~sender () {}
};

struct din;

struct mouse_sender : sender {
	din* d;
	osc_out oo;
	int prevx, prevy;
	mouse_sender (din* dd, lo_address& c, const string& p, const string& f) : d(dd), oo (c, p, f), prevx(-1), prevy(-1) {}
	bool operator()();
};

struct pitch_sender : sender {
	
	din* d;
	osc_out oo;
	float prev;
	pitch_sender (din* dd, lo_address& c, const string& p, const string& f) : d(dd), oo (c, p, f), prev (-1) {}
	bool operator()();
	
};

struct sa_sender : sender {
	
	din* d;
	osc_out oo;
	float prev;
		
	sa_sender (din* dd, lo_address& c, const string& p, const string& f) : d(dd), oo (c, p, f) {}
	bool operator()();
	
};

struct win_sender : sender {
	
	din* d;
	osc_out oo;
	int pleft, pbottom;
	int pright, ptop;
		
	win_sender (din* dd, lo_address& c, const string& p, const string& f) : d(dd), oo (c, p, f), pleft(0), pbottom(0), pright(0), ptop(0) {}
	bool operator()();
	
};

struct volume_sender : sender {
	
	din* d;
	osc_out oo;
	float prev;
		
	volume_sender (din* dd, lo_address& c, const string& p, const string& f) : d(dd), oo (c, p, f), prev(-1) {}
	bool operator()();
	
};

void osc ();

#endif
