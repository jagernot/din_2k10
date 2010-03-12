#include "audio.h"
#include <string.h>

paudio::paudio() {
  err = Pa_Initialize ();
  if (err != paNoError) cout << "portaudio init failed: no audio today" << endl;
}

paudio::~paudio () {
  err = Pa_Terminate ();
  if (err != paNoError) cout << "portaudio terminate failed" << endl; else cout << "portaudio shutdown." << endl;
}

audio_out::audio_out (audio_callback callb, void* data) {
	open (callb, data);
}

void audio_out::open (audio_callback callb, void* data) {
	extern int SAMPLE_RATE;
	err = Pa_OpenDefaultStream (&str, 0, 2, paFloat32, SAMPLE_RATE, CHANNEL_BUFFER_SIZE, /*paFramesPerBufferUnspecified,*/ callb, data);
	if (err != paNoError) {
		int ndevices;
		ndevices = Pa_GetDeviceCount ();
		PaStreamParameters outp;
		bzero (&outp, sizeof(outp));
		const PaDeviceInfo* devinfo;
		for (int i = 0; i < ndevices; ++i) {
			devinfo = Pa_GetDeviceInfo (i);
			if (devinfo->name == (string) "dmix") { // standard alsa mixer
				outp.device = i; outp.channelCount = devinfo->maxOutputChannels; outp.sampleFormat = paFloat32; outp.suggestedLatency = devinfo->defaultLowOutputLatency;
				err = Pa_OpenStream (&str, 0, &outp, SAMPLE_RATE, CHANNEL_BUFFER_SIZE, paNoFlag, callb, data);
				if (err == paNoError) {
					err = Pa_StartStream (str);
					if (err == paNoError) {
						cout << "***audio output on: " << devinfo->name << endl;
						Pa_StopStream (str);
						break;
					}
				}
			}
		}
	} else cout << "***opened audio on: default" << endl;
}

void audio_out::start () {
  if (str) {
		Pa_StartStream (str);
		cout << "started output audio stream" << endl;
	}
}

int audio_out::stop () {
  
  if (str) {
		Pa_StopStream (str);
		cout << "stopped output audio stream" << endl;
		return 1;
	} else return 0;
	
}
