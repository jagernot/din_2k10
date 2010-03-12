#ifndef _CURVE
#define _CURVE

#include <crvpt.h>
#include <list>
using namespace std;

struct curve {

  float x[2], y[2]; // start/end points
  float tx[2], ty[2]; // start/end tangents
  list <crvpt> pts;
  
  bool eval_state;
  enum {EVAL_REQUIRED, EVAL_COMPLETE};
  
  float distance2;
      
  public:
  
    curve ();
    curve (float x1, float y1, float x2, float y2, float tx1, float ty1, float tx2, float ty2, float d = 1);
    void set_distance (float d);
    
    void eval ();
    
    inline void tangent (int i, float xt, float yt) {
      tx[i] = xt;
      ty[i] = yt;
      eval_state = EVAL_REQUIRED;
    }

    inline void get_tangent (int i, float& xt, float& yt) {
      xt = tx[i];
      yt = ty[i];
    }

    inline void vertex (int i, float xv, float yv) {
      x[i] = xv;
      y[i] = yv;
      eval_state = EVAL_REQUIRED;
    }

    inline void get_vertex (int i, float& xv, float& yv) {
      xv = x[i];
      yv = y[i];
    }
    
    inline bool eval_required () {
      return (eval_state == EVAL_REQUIRED);
    }
        
};

#endif


