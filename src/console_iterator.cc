#include <console_iterator.h>

console_iterator::console_iterator (console& c, char dl) : con (c), delim (dl) {}

console_iterator& console_iterator::operator++ () {
  con << delim;
  return *this;
}

console_iterator& console_iterator::operator++ (int) {
  con << delim;
  return *this;
}

console_iterator& console_iterator::operator* () {
  return *this;
}
