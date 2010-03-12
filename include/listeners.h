#ifndef __LISTENERS
#define __LISTENERS

#include "curve_listener.h"

struct din;

struct strength_listener : curve_listener {
  
  din* d;
  strength_listener (din* dd);
  void selected (int i);
  void edited (int i);
  void disabled (int i, bool state);
};

struct wave_listener : curve_listener {
  
  din* d;
  wave_listener (din* dd);
  void edited (int i);
};

struct left_listener : curve_listener {
  
  din* d;
  left_listener (din* dd);
  void edited (int i);
  
};

struct right_listener : curve_listener {
  
  din* d;
  right_listener (din* dd);
  void edited (int i);
  
};

struct drone_listener : curve_listener {
  din* d;
  drone_listener (din* dd);
  void edited (int i);
  
};

struct attack_decay_listener : curve_listener {
  din* d;
  attack_decay_listener (din* dd);
  void edited (int i);
  
};

struct mod_listener : curve_listener {
  
  din* d;
  mod_listener (din* dd);
  void edited (int i);
};

struct beat2value;
struct beat2value_listener : curve_listener {
  beat2value* beat2value0;
  beat2value_listener (beat2value* g);
  void edited (int i);
};

#endif
