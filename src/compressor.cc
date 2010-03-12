#include "compressor.h"

compressor::compressor (const string& f) : crv (f), lis (*this) {
	fname = f;
	apply (&crv);
}

void compressor_listener::edited (int i) {
	c.apply (&c.crv);
}

compressor::~compressor () {
	crv.save (fname);
}
