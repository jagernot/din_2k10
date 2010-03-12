#include <chrono.h>
#include <audio.h>
#include <tick.h>

chrono::chrono (int tks) : tk_sec (tks) {
	
	tk_callb = audio_out::CHANNEL_BUFFER_SIZE;
	tk = 0;
	
	t = 0;
	dt = tk_callb * 1. / tk_sec;
	
}

chrono& chrono::operator++ () {
	
	tk += tk_callb;
	t += dt;
	return *this;
	
	
}

int chrono::sec2tk (float s) {
	return (s * tk_sec + 0.5);
}

extern chrono clk;

timer::timer (int nt, float ns) : start(-1), duration (-1), ntrigs (nt), nsecs (ns), state (disabled)  {}

void timer::calc_duration () {
	int nticks = clk.sec2tk (nsecs);
	duration = nticks / ntrigs; // int for fast check
}

void timer::go () {
	start = clk.tk - duration;
	state = running;
}

int timer::eval () {
	
	int end = clk.tk;
	int elapsed = end - start;
	int tip = elapsed - duration;
	if (tip >= 0) {
		start = clk.tk + tip;
		return 1;
	}
	
	return 0;
	
}
