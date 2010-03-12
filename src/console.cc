#include "console.h"
#include "font.h"
#include "input.h"
#include "command.h"

decoration::decoration (float rr, float gg, float bb, int w) {
  r = rr;
  g = gg;
  b = bb;
  lw = w;
}

const float cc = 0.6;
const decoration console::light = decoration ();
const decoration console::yellow = decoration (1, 1, cc);
const decoration console::green = decoration (cc, 1, cc);
const decoration console::red = decoration (1, cc, cc);
const decoration console::cyan = decoration (cc, 1, 1);
const decoration console::bold = decoration (0, 0, 0, 2);

void console::push_back (const mesg& ln) {

  lines.push_back (ln);
  ++nlines;
  last ();
  
}

void console::up (int i) {
	startl = max (0, startl - i);
}

void console::down (int i) {
  startl += i;	
  if (startl >= nlines) startl = max (0, nlines - 1);
}

void console::last () {
  // ensures last line is always displayed
  end ();
  if (!rollup) up (lines_per_screen - 2);
}

void console::pgdn () {
  down (lines_per_screen);
}

void console::pgup () {
  up (lines_per_screen);
}

void console::home () {
  startl = 0;
}

void console::end () {
  startl = max (0, nlines - 1);
}

void console::add_last_line () {
  push_back (last_line);
  last_line = mesg();
}

console::console () {
  rollup = false;
  command_mode = 0;
  hid = -1;
	ps1 = "# ";
  clear ();
}

console& console::operator<< (unsigned int i) {
  sprintf (charbuf, "%x", i);
  last_line.text += charbuf;
  return *this;
}

console& console::operator<< (int i) {
  sprintf (charbuf, "%d", i);
  last_line.text += charbuf;
  return *this;
}

console& console::operator<< (unsigned long  i) {
  sprintf (charbuf, "%lu", i);
  last_line.text += charbuf;
  return *this;
}

console& console::operator<< (unsigned long long i) {
  sprintf (charbuf, "%llux", i);
  last_line.text += charbuf;
  return *this;
}

console& console::operator<< (const string& s) {
  last_line.text += s;
  return *this;
}

console& console::operator<< (float f) {
  sprintf (charbuf, "%f", f);
  last_line.text += charbuf;
  return *this;
}

console& console::operator<< (double d) {
  sprintf (charbuf, "%f", d);
  last_line.text += charbuf;
  return *this;
}

console& console::operator<< (char c) {

  switch (c) {

    case eol:
      add_last_line ();
      break;

    case cll:
      last_line = mesg();
      break;

    default:
      last_line.text += c;
      break;
  }
  return *this;
}

void console::del () {
	
	string s = last_line.text.substr (0, last_line.text.length() - 1);
	last_line.text = s;
	
}

console& console::operator<< (const decoration& d) {
  last_line.dec = d;
  return *this;
}

void console::draw () {
  
  glMatrixMode (GL_MODELVIEW);
  
  glPushMatrix ();
  glLoadIdentity ();

  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();
  glLoadIdentity ();
  glOrtho (win.left, win.right, win.bottom, win.top, -1, 1);
  
  // draw lines
  int mcw = max_char_width(), lh = get_line_height ();
  int cx = win.left + (ps1.length()) * mcw, cy = win.top - lh;
	
  int n = 0, i = startl;
  while ((n++ < lines_per_screen) && (i < nlines)) {
    const mesg& linei = lines[i++];
    const decoration& dec = linei.dec;
    glColor3f (dec.r, dec.g, dec.b);
		draw_string (ps1, win.left, cy);
    draw_string (linei.text, cx, cy);
    cy -= lh;
  }
	
  // draw last line
  glColor3f (1, 1, 1);
	draw_string (ps1, win.left, cy);
  int px = draw_string (last_line.text, cx, cy, 0);
  
	// draw cursor
  if (command_mode) {  	
  	glColor3f (1, 0.1, 0.1);
  	glRecti (px, cy, px + mcw, cy + 2);
	}
  
  glPopMatrix ();

  glMatrixMode (GL_MODELVIEW);
  glPopMatrix ();


}

void console::clear () {
  lines.clear ();
  last_line = mesg();
  nlines = 0;
  startl = 0;
}

void console::calc_lines_per_screen () {
  lines_per_screen = (win.top - win.bottom) / get_line_height ();
}

void console::set_window (const box<int>& w) {
  win = w;
	calc_lines_per_screen ();
}

bool console::handle_input () {
	
  if (command_mode) {
		
  	static const string cch [] = {
  		"abcdefghijklmnopqrstuvwxyz0123456789 .=-/;<\\,[]'`",
  		"ABCDEFGHIJKLMNOPQRSTUVWXYZ)!@#$%^&*( >+_?:<|<{}\"~"
  	};
  	static const char* cch0 = cch[0].c_str();
  	static const char* cch1 = cch[1].c_str();
		static const int j = cch[0].length ();
		
  	for (int i = 0; i < j; ++i) {
  		if (keypressedd (cch0[i])) {
  			if (keydown (SDLK_LSHIFT) || keydown (SDLK_RSHIFT) || keydown (SDLK_CAPSLOCK)) 
  				*this << cch1[i];
  			else
  				*this << cch0[i];
  		}
		}

		if (keypressed (SDLK_RETURN)) {
			command = last_line.text;
			add_last_line ();
			int last = command.length () - 1;
			if ((last > 0) && (command[last] == ';')) {
				command = command.substr (0, last); // strip ; 
				command_mode = 1;
			} else command_mode = 0;
			extern cmdlist cmdlst;
			cmdlst.run (command);
			history.push_back (command);
			hid = history.size () - 1;
		}
		
		if (hid > -1) {
			if (keypressedd (SDLK_UP)) {
				last_line.text = history [hid--]; 
				if (hid < 0) hid = 0;
			}
			if (keypressedd (SDLK_DOWN)) {
				last_line.text = history [hid++];
				int hs = history.size ();
				if (hid >= hs) hid = hs - 1;
			}
		}

	}
	
	if (keypressed (SDLK_TAB)) {
		last_line = mesg();
		command_mode = !command_mode;
	}
	
				
  if (keypressedd (SDLK_INSERT)) up(1);
  if (keypressedd (SDLK_DELETE)) down(1);
  if (keypressed (SDLK_HOME)) home ();
  if (keypressed (SDLK_END)) end();
  static clock_t pg0 = secs2ticks(1./2), pg1 = secs2ticks(1./16);
  if (keypressedd (SDLK_PAGEUP, pg0, pg1)) pgup();
  if (keypressedd (SDLK_PAGEDOWN, pg0, pg1)) pgdn();
  if (keypressedd (SDLK_BACKSPACE)) {
		if (command_mode) del (); else clear();
  }
  if (keypressed ('`') && !command_mode) {
		rollup = !rollup;
		last ();
	}
  return true;
  
}

console::~console () {}
