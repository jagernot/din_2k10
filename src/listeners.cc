#include "din.h"
#include "listeners.h"
#include "beat2value.h"

strength_listener::strength_listener (din* dd) : d (dd) {}

void strength_listener::selected (int i)  {
  d->select_resonator (i);
}

void strength_listener::edited (int i) {
  d->edit_resonator (i, resonator::STRENGTH);
}

void strength_listener::disabled (int i, bool state) {
  d->disable_resonator (i, state);
}

wave_listener::wave_listener (din* dd) : d (dd) {}

void wave_listener::edited (int i) {
  d->edit_resonator (d->cur_resonator, resonator::WAVE);
}

left_listener::left_listener (din* dd) : d (dd) {}

void left_listener::edited (int i) {
  d->edit_resonator (d->cur_resonator, resonator::LEFT);
}

right_listener::right_listener (din* dd) : d (dd) {}

void right_listener::edited (int i) {
  d->edit_resonator (d->cur_resonator, resonator::RIGHT);
}

drone_listener::drone_listener (din* dd) : d (dd) {}

void drone_listener::edited (int i) {
	d->drone_sol.init ();
}

beat2value_listener::beat2value_listener (beat2value* bv) : beat2value0 (bv) {}

void beat2value_listener::edited (int i) {
  beat2value0->curve_edited ();
}
