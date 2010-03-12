#ifndef __tick
#define __tick

#include <time.h>

inline double ticks2secs (clock_t ticks) {
	return ticks * 1.0 / CLOCKS_PER_SEC;
}

inline clock_t secs2ticks (double secs) {
	return (clock_t) (secs * CLOCKS_PER_SEC);
}

#endif
