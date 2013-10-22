
#include <iostream>

#include <pulse/error.h>
#include <sndfile.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

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
    
    int ret = pa_context_connect(paContext,"localhost",(pa_context_flags_t)0, NULL);
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
    
    
    
    // we decode the wav header (44 bytes long)
    SF_INFO sfinfo;
    sf_userdata user;
    user.curPos = 0;
    user.data = sound_test_wav;
    user.filelen = sound_test_wav_size;
    SF_VIRTUAL_IO sf_vio;
    sf_vio.get_filelen = &Sound::sf_vio_get_filelen;
    sf_vio.read = &Sound::sf_vio_read;
    sf_vio.tell = &Sound::sf_vio_tell;
    sf_vio.seek = &Sound::sf_vio_seek;
    SNDFILE* sf = sf_open_virtual(&sf_vio, SFM_READ, &sfinfo, &user);
    if(sf==NULL) {
        cout << "SndFile failed: " << sf_strerror (sf) << endl;
    }
    sf_close(sf);
    int sf_format_sub = SF_FORMAT_SUBMASK & sfinfo.format;
    int sf_format_type = SF_FORMAT_TYPEMASK & sfinfo.format;
    // endianness seems to be on default file endianness, which
    // is little for Microsoft WAV files
    // int sf_format_end = SF_FORMAT_ENDMASK & sfinfo.format;
    
    pa_sample_format_t format = PA_SAMPLE_INVALID;    
    if(sf_format_type != SF_FORMAT_WAV) {
        cerr << "Loaded file not WAV!" << endl;
        exit(EXIT_FAILURE);
    }
    else if(sf_format_sub == SF_FORMAT_PCM_U8) {
        format = PA_SAMPLE_U8;        
    }
    else if(sf_format_sub == SF_FORMAT_PCM_16) {
        format = PA_SAMPLE_S16LE;        
    }
    else if(sf_format_sub == SF_FORMAT_PCM_24) {
        format = PA_SAMPLE_S24LE;        
    }
    
    if(format == PA_SAMPLE_INVALID) {
        cerr << "WAV Format not supported..." << endl;
        exit(EXIT_FAILURE);
    }   
               
    const pa_sample_spec ss = {
        format,
        (uint32_t)sfinfo.samplerate, 
        (uint8_t)sfinfo.channels 
    };
    
	paStream = pa_stream_new(paContext,"PiGLETStream",&ss, NULL);

	if(NULL==paStream) {
		cerr << "Cannot create PulseAudio stream." << cerr;
	}
    
    pa_cvolume cv;    
    ret = pa_stream_connect_playback(paStream,NULL,NULL,(pa_stream_flags_t)0,pa_cvolume_reset(&cv, ss.channels),NULL);
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
            cout << "Underflow (maybe playback done)" << endl;
            break;
		}
               
        if(i==max_timeouts) {
            cerr << "Stream could not be written (too many tries)" << endl;
            exit(EXIT_FAILURE);
        }
        usleep(10000); // wait 10 ms
    }
       
    pa_mainloop_iterate(paMainLoop, 0, NULL);    
       
}

Sound::~Sound()
{
    pa_stream_disconnect(paStream);
	pa_stream_unref(paStream);
    pa_context_disconnect(paContext);
	pa_context_unref(paContext);
	pa_mainloop_free(paMainLoop);  
}


sf_count_t Sound::sf_vio_tell(void *user_data)
{
    return static_cast<sf_userdata*>(user_data)->curPos;
}

sf_count_t Sound::sf_vio_read(void *ptr, sf_count_t count, void *user_data)
{
    sf_userdata* data = static_cast<sf_userdata*>(user_data);
    memcpy(ptr, data->data+data->curPos, count);
    data->curPos += count;
    return count;
}


sf_count_t Sound::sf_vio_seek(sf_count_t offset, int whence, void *user_data)
{
    sf_userdata* data = static_cast<sf_userdata*>(user_data);
    switch (whence) {
    case SEEK_CUR:
        data->curPos += offset;
        break;
    case SEEK_SET:
        data->curPos = offset;
        break;
    case SEEK_END:
        data->curPos = data->filelen-offset;
        break;
    default:
        break;
    }    
    return data->curPos;
}


sf_count_t Sound::sf_vio_get_filelen(void *user_data)
{
    return static_cast<sf_userdata*>(user_data)->filelen;
}
