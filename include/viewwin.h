#ifndef __viewwin
#define __viewwin

#include <box.h>
#include <string>
using namespace std;

struct badviewport {};

struct viewport {
  
	int cx, cy; // center
	int xmax, ymax;
	float xmax_1, ymax_1; // 1.0 / xmax , 1.0 / ymax
  int width, height;	
	
	viewport (const string& name, int width, int height, int depth);
	viewport ();
	bool inside (int x, int y);
	
	
};


//
// window
//

class window : public box<float> {

	float pan_x, pan_y;
	static const float pan_scale = 0.004;
	static const float zoom_scale = 0.003;

	void calc_panxy ();
	
	float width_1, height_1; // 1.0 / width, 1.0 / height
	float aspect_ratio;

	public:
	
		window ();
		window (float l, float b, float r, float t);
    void set (float l, float b, float r, float t);
    void get (float& l, float& b, float& r, float& t);
		void panx (int dir);
		void pany (int dir);
		void zoom (int dir);
		void project ();
		
  friend void win2view (float wx, float wy, int& vx, int& vy, window& win, viewport& view);
  
};

inline void view2win (int vx, int vy, float& wx, float& wy, viewport& view, window& win) {

	float xr = vx * view.xmax_1;
	float yr = 1 - vy * view.ymax_1;

	wx = win.left + xr * win.width;
	wy = win.bottom + yr * win.height;

}

inline void win2view (float wx, float wy, int& vx, int& vy, window& win, viewport& view) {
	float xr = (wx - win.left) * win.width_1;
	float yr = (wy - win.bottom) * win.height_1;
	vx = (int) (xr * view.xmax);
	vy = (int) (yr * view.ymax);

}

#endif
