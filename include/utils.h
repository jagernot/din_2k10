#ifndef _UTILS
#define _UTILS

template <typename T> inline bool inrange (const T& min, const T& val, const T& max) {
	return ((val >= min) && (val <= max));
}

template<typename T> inline T& clip (const T& lo, T& val, const T& hi) {

	// ensures lo <= val <= hi
	if (val < lo) val = lo; else if (val > hi) val = hi;
	return val;

}
#endif
