#ifndef __box
#define __box

template <typename T> struct box {
	T left, bottom;
	T right, top;
	T width, height;
	double width_1, height_1;

	box () {
		left = bottom = right = top = width = height = width_1 = height_1 = 0;
	}
  
	box (T l, T b, T r, T t) {
		operator() (l, b, r, t);
	}

	inline void operator() (T l, T b, T r, T t) {
		left = l;
		bottom = b;
		right = r;
		top = t;
    calc_width_height ();
	}

  inline void calc_width_height () {
    width = right - left;
    height = top - bottom;
		if (width != 0) width_1 = 1.0 / width;
		if (height != 0) height_1 = 1.0 / height;
  }
	
	inline void move (T dx, T dy) {
		left += dx;
		right += dx;
		bottom += dy;
		top += dy;
	}
	
	inline void lower_corner (T x, T y) {
		left = x;
		bottom = y;
		right = left + width;
		top = bottom + height;
	}

};

template <typename T> inline bool inbox (const box<T>& b, const T& x, const T& y) {
	return ((x >= b.left) && (x <= b.right) && (y >= b.bottom) && (y <= b.top));
}

#endif

