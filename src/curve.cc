#include <curve.h>
#include <vector2d.h>
#include <utils.h>
#include <color.h>
#include <iostream>
using namespace std;

curve::curve () {
  for (int i = 0; i < 2; ++i) x[i] = y[i] = tx[i] = ty[i] = 0;
  set_distance (1);
}

curve::curve (float x1, float y1, float x2, float y2, float tx1, float ty1, float tx2, float ty2, float d) {
  x[0] = x1; x[1] = x2;
  y[0] = y1; y[1] = y2;
  tx[0] = tx1; tx[1] = tx2;
  ty[0] = ty1; ty[1] = ty2;
  set_distance (d);
}

void curve::set_distance (float d) {
  distance2 = d * d;
  eval_state = EVAL_REQUIRED;
}

void curve::eval() {
	
	pts.clear ();
		
	pts.push_back (crvpt (x[0], y[0], 0));
	pts.push_back (crvpt (x[1], y[1], 1));
	
	list<crvpt>::iterator i = pts.begin ();
	list<crvpt>::iterator j = ++pts.begin();
	
	crvpt r;
	while (j != pts.end()) {	
		
		crvpt& p = *i;
		crvpt& q = *j;
		
		float T = (p.t + q.t) / 2;
		float T1 = 1 - T;
		float T1_2 = T1 * T1;
		float T1_3 = T1_2 * T1;
		float T_2 = T * T;
		float T_3 = T_2 * T;
		float BP1 = T1_3;
		float BT1 = 3 * T * T1_2;
		float BT2 = 3 * T_2 * T1;
		float BP2 = T_3;

		float sx = BP1 * x[0] + BT1 * tx[0] + BT2 * tx[1] + BP2 * x[1];
		float sy = BP1 * y[0] + BT1 * ty[0] + BT2 * ty[1] + BP2 * y[1];

		float mx = (p.x + q.x) / 2, my = (p.y + q.y) / 2;

		float d2 = magnitude2 (mx, my, sx, sy);
		
		if (d2 >= distance2) {
			r.x = sx; r.y = sy; r.t = T; r.m = 0; r.inf = 0;
			j = pts.insert (j, r);
		} else {
			float dx = q.x - p.x;
			if (dx == 0) {
				p.m = 0; 
				p.inf = 1;
			} else {
				float dy = q.y - p.y;
				if (dy == 0) {
					p.m = 0;
					p.inf = 0;
				} else {
					p.m = dy / dx;
					p.inf = -1;
				}
			}
			i = j;
			++j;
		}
		
	}
		
	eval_state = EVAL_COMPLETE;
	
}
