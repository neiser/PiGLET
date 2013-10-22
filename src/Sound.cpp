
#include <iostream>

#include <pulse/stream.h>
#include <pulse/error.h>
#include <stdlib.h>
#include <unistd.h>

#include "Sound.h"
extern "C" {
#include "wavfiles.h"
}

using namespace std;

void Sound::Play()
{
    
}

Sound::Sound()
{
    pa_mainloop *paMainLoop = pa_mainloop_new();  // Pennsylvania Main loop?
	if(NULL==paMainLoop)
	{
		cerr << "Cannot create PulseAudio main loop." << endl;
        exit(EXIT_FAILURE);
	}

	pa_context *paContext = pa_context_new(pa_mainloop_get_api(paMainLoop),"YsPulseAudioCon");
	if(NULL==paContext)
	{
		cerr << "Cannot create PulseAudio context" << endl;
        exit(EXIT_FAILURE);
	}
    
    cout << "PulseAudio mainloop and context created." << endl;
    
    int ret = pa_context_connect(paContext,NULL,(pa_context_flags_t)0, NULL);
    if(ret != PA_OK) {
        cerr << "PulseAudio context connect failed" << endl;
        exit(EXIT_FAILURE);
    }

    // poll the state with timeout of about 1 s
    for(size_t i=0;i<10;i++) {
        // this still fail
        pa_mainloop_iterate(paMainLoop,0,NULL);
		if(PA_CONTEXT_READY==pa_context_get_state(paContext))
		{
			break;
		}
        else if(i==9) {
            cerr << "Connection to PulseAudio timed out" << endl;
            exit(EXIT_FAILURE);
        }
        usleep(100000);
    }
    
    cout << "PulseAudio connected." << endl;
    
    const pa_sample_spec ss = {
        PA_SAMPLE_S16LE, // format
        44100, // rate
        2 // nChannel
    };

	pa_stream *paStream = pa_stream_new(paContext,"PiGLETStream",&ss,NULL);

	if(NULL==paStream) {
		cerr << "Cannot create PulseAudio stream." << cerr;
	}

	ret = pa_stream_connect_playback(paStream,NULL,NULL,(pa_stream_flags_t)0,NULL,NULL);
    if(ret != PA_OK) {
        cerr << "PulseAudio stream connect playback failed" << endl;
        exit(EXIT_FAILURE);
    }
    
    cout << sizeof(sound_test_wav[1]) << sound_test_wav[1] << endl;
}

Sound::~Sound()
{
    
}
