
#include <iostream>

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
    paMainLoop = pa_mainloop_new();  // Pennsylvania Main loop?
	if(NULL==paMainLoop)
	{
		cerr << "Cannot create PulseAudio main loop." << endl;
        exit(EXIT_FAILURE);
	}

	paContext = pa_context_new(pa_mainloop_get_api(paMainLoop),"PiGLETPulseAudioCon");
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

    // poll the state with timeout of about 1000 ms
    size_t max_timeouts = 1000;
    for(size_t i=1;;i++) {
        // this still fail
        pa_mainloop_iterate(paMainLoop,0,NULL);
        pa_context_state_t state = pa_context_get_state(paContext);
		if(state==PA_CONTEXT_READY)
		{
			break;
		}
        else if(i==max_timeouts) {
            cerr << "Connection to PulseAudio timed out: "<< 
                    state << endl;
            exit(EXIT_FAILURE);
        }
        usleep(1000); // wait 1 ms
    }
    
    cout << "PulseAudio connected." << endl;
    
    const pa_sample_spec ss = {
        PA_SAMPLE_S16LE, // format
        44100, // rate
        1 // nChannel
    };

	paStream = pa_stream_new(paContext,"PiGLETStream",&ss,NULL);

	if(NULL==paStream) {
		cerr << "Cannot create PulseAudio stream." << cerr;
	}

    ret = pa_stream_connect_playback(paStream,NULL,NULL,(pa_stream_flags_t)0,NULL,NULL);
    if(ret != PA_OK) {
        cerr << "PulseAudio stream connect playback failed" << endl;
        exit(EXIT_FAILURE);
    }
    
    // feed some data into the stream
    size_t cur = 44;
    for(size_t i=0;;) {
      
		if(PA_STREAM_READY==pa_stream_get_state(paStream)) {
            const size_t writableSize = pa_stream_writable_size(paStream);
            const size_t sizeRemain = sound_test_wav_size - cur;
            const size_t writeSize = sizeRemain<writableSize ? sizeRemain : writableSize;
            cout << "Ready: Writable " 
                 << writableSize << " Remain " 
                 << sizeRemain << " " << " Size " 
                 << writeSize << endl;
            if(writeSize>0) {
                pa_stream_write(paStream,&sound_test_wav[cur],writeSize,NULL,0,PA_SEEK_RELATIVE);
                cur += writeSize;
            }
		}
        else  {
            cout << "Timeout..."<< i << endl;
            i++;
        }
        
        pa_mainloop_iterate(paMainLoop,0,NULL);
        
        if(sound_test_wav_size<=cur ||
		   0<=pa_stream_get_underflow_index(paStream))
		{
            cout << "Underflow (playback done)" << endl;
            break;
		}
               
        if(i==max_timeouts) {
            cerr << "Stream could not be written (too many tries)" << endl;
            exit(EXIT_FAILURE);
        }
        usleep(10000); // wait 10 ms
    }
       
    pa_mainloop_iterate(paMainLoop,0,NULL);    
       
    //usleep(1000000);
}

Sound::~Sound()
{
    pa_stream_disconnect(paStream);
	pa_stream_unref(paStream);
    pa_context_disconnect(paContext);
	pa_context_unref(paContext);
	pa_mainloop_free(paMainLoop);  
}
