#ifndef CURVE_LISTENER
#define CURVE_LISTENER
struct curve_listener {
  virtual void selected (int i) {}
  virtual void edited (int i)=0;
  virtual void disabled (int i, bool state) {}
  virtual ~curve_listener () {}
};
#endif
