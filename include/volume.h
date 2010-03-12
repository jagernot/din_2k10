#ifndef __VOLUME
#define __VOLUME

#include <stereo.h>

struct volume {

  // volume of each resonator
  stereo res [resonator::MAX_RESONATORS];

};

#endif
