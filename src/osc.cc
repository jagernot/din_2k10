#include "osc.h"
#include "din.h"
#include "console.h"
#include "command.h"

bool oid::operator() (stringstream& ss) {
	ss >> name;
	cons << console::yellow << name << " has logged into din." << eol;
	eresult = "hello, " + name;
	return true;
}

bool oserver::operator() (stringstream& ss) {
	
	string port; ss >> port; 
	float msecs; ss >> msecs;
	if (msecs <= 0) msecs = 1;
	
	if (server) lo_server_free (server);
	server = lo_server_new (port.c_str(), 0);
	lo_server_add_method (server, 0, 0, command_handler, 0);
	
	lo_fd = lo_server_get_socket_fd (server);
	if (lo_fd) {
		cons << console::green << "server started at " << port << ", command timeout is " << msecs << " msecs." << eol;
		tv.tv_sec = 0;
		tv.tv_usec = msecs * 1000;
		enabled = true;
		msg = true;
		return true;
	}
	
	return false;
	
}

void oserver::handle_requests () {
	if (lo_fd > 0) {
		FD_ZERO (&rfds);
		FD_SET(lo_fd, &rfds);
		int retval = select(lo_fd + 1, &rfds, 0, 0, &tv);
		if (retval == -1) return; 
		lo_server_recv_noblock(server, 0);
		//if (retval > 0 && FD_ISSET (lo_fd, &rfds)) lo_server_recv_noblock(server, 0);
	}
}

void oserver::toggle () {
	enabled = !enabled;
}

oserver::~oserver () {
	
	if (server) lo_server_free (server);
	
}

int command_handler (const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
	
	extern oserver srv;
	if (!srv.enabled) return 0;
	
	stringstream ss (path);
	string first; ss >> first;
	if (first == "/failure" || first == "/success") return 0;
		
	extern cmdlist cmdlst;
	bool errmsg = false;
	command* cmd = cmdlst.run (path, errmsg);
	
	extern oid ido;
	if (cmd == 0) {
		if (srv.msg) {
			cons << console::red;
			if (ido.name != "") cons << '@' << ido.name << ':'; 
			cons << " bad command <" << path << '>' << eol;
		}
		cmdlst.run ("send /failure"); 
	} else {
		if (srv.msg) {
			cons << console::cyan;
			if (ido.name != "") cons << '@' << ido.name << ':'; 
			cons << cll << " ran <" << path << ">, result: " << cmd->eresult;
		}
		cmdlst.run ("send /success " + cmd->eresult);
	}
	return 1;
}

bool oconnect::operator() (stringstream& ss) {
	
	string ip, port;
	ss >> ip >> port;
	
	client = lo_address_new (ip.c_str(), port.c_str());
	if (!client) {
		cons << console::red << "failed to connect: " << ip << " at " << port << eol;
		return false;
	} else cons << console::green << "connected to: " << ip << " at " << port << eol;
	return true;
}

bool osend::operator() (stringstream& ss) {
	
	extern int MAX_VARS;
	extern sender* sndr[];
	extern string vars[];
	
	string var; ss >> var;
	
	if (client) {
		
		for (int i = 0; i < MAX_VARS; ++i) {
			if (var == vars[i]) {
				(*sndr[i])();
				return true;
			}
		}
		
		string word, last_word;
		while (1) {
			ss >> word;
			if (word == last_word) break;
			var = var + ' ' + word;
			last_word = word;
		}
		lo_send (client, var.c_str(), "s", var.c_str());
		return true;
		
	} else {
		cons << console::red << "connect to osc server first" << eol;
		return false;
	}
}

bool otransmit::operator() (stringstream& ss) {
	
	extern int MAX_VARS;
	extern string vars[];
	extern timer tmrs [];
	
	string var;
	int ntrigs;
	float nsecs;
	
	ss >> var >> ntrigs >> nsecs;
	
	if (var == "") {
		for (int i = 0; i < MAX_VARS; ++i) {
			if (tmrs[i].state == timer::running) cons << console::green; else cons << console::red;
			cons << "name: " << vars[i] << "  ntrigs: " << tmrs[i].ntrigs << "  nsecs: " << tmrs[i].nsecs << eol;
		}
		return true;
	}

	if (!client) {
		cons << console::red << "please connect to an osc server first." << eol;
		return false;
	}
	
	for (int i = 0; i < MAX_VARS; ++i) {
		if (var == vars[i]) {
			timer& ti = tmrs[i];
			ti.ntrigs = ntrigs;
			ti.nsecs = nsecs;
			if ((ti.ntrigs >= 0) && (ti.nsecs > 0)) {
				ti.calc_duration ();
				ti.go ();
				cons << console::green << "transmitting: " << var << eol;
				return true;
			} else {
				ti.state = timer::disabled;
				cons << console::yellow << "stopped transmitting: " << var << eol;
				return true;
			}
		}
	}
	
	cons << console::red << "osc var: " << var << " not found." << eol;
	return false;
	
}

bool mouse_sender::operator() () {
	if (oo.client && ((d->prev_mousex != d->win_mousex) || (d->prev_mousey != d->win_mousey))) {
		lo_send (oo.client, oo.path.c_str(), oo.format.c_str(), d->win_mousex, d->win_mousey);
		return true;
	} else return false;
}

bool pitch_sender::operator() () {
	float step = d->step;
	if ((step != prev) && oo.client) {
		extern int SAMPLE_RATE;
		float pitch = step * SAMPLE_RATE;
		prev = step;
		lo_send (oo.client, oo.path.c_str(), oo.format.c_str(), pitch);
		return true;
	} else return false;
}

bool sa_sender::operator()() {
	float sa = d->get_sa ();
	if ((sa != prev) && oo.client) {
		lo_send (oo.client, oo.path.c_str(), oo.format.c_str(), sa);
		prev = sa;
		return true;
	}
	return false;
}

bool win_sender::operator()() {
	if (oo.client) {
		extern viewport view;
		int left = d->ranges[0].extents.left;
		int right = d->ranges[d->num_ranges - 1].extents.right;
		int bottom = 0, top = view.ymax;
		if ((left != pleft) || (right != pright) || (bottom != pbottom) || (top != ptop)) {
			lo_send (oo.client, oo.path.c_str(), oo.format.c_str(), left, bottom, right, top);
			pleft = left; pbottom = bottom;
			pright = right; ptop = top;
			return true;
		} 
	}
	return false;
	
}

bool volume_sender::operator()() {
	if (oo.client) {
		extern int LAST_VOLUME;
		float volume = d->dv * 1./ LAST_VOLUME;
		if (volume < 0) volume = 0;
		if (volume != prev) lo_send (oo.client, oo.path.c_str(), oo.format.c_str(), volume);
		prev = volume;
		return true;
	} else return false;
	
}

void osc () {
	
	extern oserver srv;
	srv.handle_requests ();
	
	extern int MAX_VARS;
	extern timer tmrs[];
	extern sender* sndr[];
	for (int i = 0; i < MAX_VARS; ++i) {
		timer& t = tmrs[i];
		if (t.state != timer::disabled) {
			if (t.eval ()) (*sndr[i])();
		}
	}
	
}


