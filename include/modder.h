#ifndef __modder
#define __modder

template <class T> 

struct modder { // for modifying param

  T min, max, delta;
  
  public:
    
    modder () {
      min = max = delta = 0;
    }
    
    modder (T _min, T _max, T _delta) {
      min = _min;
      max = _max;
      delta = _delta;
    }
        
    void inc (T& param) {
      T pi = param + delta;
      if (pi > max) pi = max; 
      param = pi;
    }
    
    void dec (T& param) {
      T pi = param - delta;
      if (pi < min) pi = min; 
      param = pi;
    }
    
};

#endif
