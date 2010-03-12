#ifndef __point
#define __point

template <class T> struct point {
	T x, y;
	point (T a, T b) : x(a), y(b) {}
	point () : x(0), y(0) {}
	void operator() (T a, T b) {
		x = a;
		y = b;
	}
	bool operator== (const point<T>& p) {
	  return ( (x == p.x) && (y == p.y));
  }
};


#endif
