#ifndef __vector2d
#define __vector2d
#include <math.h>
template <class T> inline T distance2 (const T& x1, const T& y1, const T& x2, const T& y2) {
	T dx = x2 - x1;
	T dy = y2 - y1;
	return (dx * dx + dy * dy);
}

template<class T> inline void direction (T& dx, T& dy, const T& x1, const T& y1, const T& x2, const T& y2) {
	dx = x2 - x1;
	dy = y2 - y1;
}

template <class T> inline double magnitude (const T& x1, const T& y1, const T& x2, const T& y2) {
	T dx, dy;
	direction (dx, dy, x1, y1, x2, y2);
	return sqrt ((double) (dx * dx + dy * dy));

}

template <class T> inline double magnitude (const T& vx, const T& vy) {
	return sqrt ((double) (vx * vx + vy * vy));

}

template <class T> inline double magnitude2 (const T& x1, const T& y1, const T& x2, const T& y2) {
	T dx, dy;
	direction (dx, dy, x1, y1, x2, y2);
	return (dx * dx + dy * dy);

}

template <class T> inline T magnitude2 (const T& vx, const T& vy) {
  return (vx * vx + vy * vy);
}


template<class S, class T> inline double unit_vector (S& ux, S& uy, const T& x1, const T& y1, const T& x2, const T& y2) {

	// unit vector joining x1,y1 & x2,y2
	T dx, dy;	direction (dx, dy, x1, y1, x2, y2);
	double mag = magnitude (dx, dy);
	if (mag > 0) {
		ux = dx / mag;
		uy = dy / mag;
	} else {
		ux = uy = 0;
	}
	return mag;
}

template <class T> inline double unit_vector (T& ux, T& uy, const T& vx, const T& vy) {
  
  
  double mag = magnitude (vx, vy);
	if (mag > 0) {
		ux = vx / mag;
		uy = vy / mag;
	} else {
		ux = uy = 0;
	}
  
  return mag;
  
}

#endif
