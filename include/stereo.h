#ifndef __STEREO
#define __STEREO

struct stereo {

  //
  // left and right volume
  //

  float left, right;
  stereo () {
    left = right = 0;
  }

};

#endif
