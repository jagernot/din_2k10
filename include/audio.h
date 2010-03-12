#ifndef __AUDIO
#define __AUDIO

#include <portaudio.h>
#include <iostream>
using namespace std;


struct paudio {

	// port audio init/exit

	private:
		int err;

	public:
	
	  paudio ();
	  ~paudio ();

};

struct audio_out {
	
	// output audio stream

	static const int CHANNEL_BUFFER_SIZE = 32;
	static const int STEREO_BUFFER_SIZE = 2 * CHANNEL_BUFFER_SIZE;
	static const int NUM_BUFFER_BYTES = sizeof (float) * STEREO_BUFFER_SIZE;

	PaError err;
	PaStream* str;

	public:
	
    typedef int (*audio_callback) (const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void*);
	  audio_out (audio_callback callb, void* data = 0);
		void open (audio_callback callb, void* data = 0);
	  void start ();
	  int stop ();

};

int audio_wanted (const void* ib, void* ob, unsigned long fpb,
									const PaStreamCallbackTimeInfo* tinfo,
									PaStreamCallbackFlags flags,
									void *data);

#endif
