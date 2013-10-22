
#include "Sound.h"
#include <iostream>
#include <pulse/error.h>
#include <pulse/scache.h>

#include <sndfile.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


extern "C" {
#include "wavfiles.h"
}

using namespace std;

void Sound::Play()
{
    
}

Sound::Sound()
{
    // create the loop & the context
    paMainLoop = pa_mainloop_new();
	if(NULL==paMainLoop)
	{
		cerr << "Cannot create PulseAudio main loop." << endl;
        exit(EXIT_FAILURE);
	}

	paContext = pa_context_new(pa_mainloop_get_api(paMainLoop),"PiGLETPulseContext");
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
    
    // setup the available streams
    
    pa_stream* paStream = readDataIntoStream("test", sound_alert_wav, sound_alert_wav_size);
    
    //cout << "Playing on " << pa_stream_get_device_name (paStream) << endl;

   
    pa_context_get_sample_info_list 	( paContext,
            &Sound::pa_sample_info_cb,
            NULL
        );
    
    
    pa_operation *o = pa_context_play_sample(
                paContext,
                "bell-window-system", // Name of my sample
                NULL, // Use default sink
                PA_VOLUME_NORM, // Full volume
                &Sound::pa_context_success_cb, // Don't need a callback
                NULL
                );
    while(pa_operation_get_state(o) == PA_OPERATION_RUNNING) {
        pa_mainloop_iterate(paMainLoop,0,NULL);
        usleep(100000);
        cout << "Running" << pa_operation_get_state(o)  << endl;
    }
    
    cout << "Playing on " << pa_stream_get_device_name (paStream) << endl;

    pa_operation_unref(o);
    
    usleep(100000);
    pa_stream_disconnect(paStream);
	pa_stream_unref(paStream);
   
    exit(0);
}

Sound::~Sound()
{
    
    pa_context_disconnect(paContext);
	pa_context_unref(paContext);
	pa_mainloop_free(paMainLoop);  
}

pa_stream* Sound::readDataIntoStream(const string &name, const unsigned char *data, size_t size)
{
    pa_sample_spec ss = getFormat(data, size);  
    
    pa_stream* paStream = pa_stream_new(paContext,"test",&ss, NULL);

    
    
	if(NULL==paStream) {
		cerr << "Cannot create PulseAudio stream for "<< name << cerr;
	}
        
    
    pa_cvolume cv;    
    static const size_t wav_hdr_size = 44;
    //int ret = pa_stream_connect_playback(paStream,NULL,NULL,(pa_stream_flags_t)0,pa_cvolume_reset(&cv, ss.channels),NULL);
    int ret = pa_stream_connect_upload(paStream,size-wav_hdr_size);
    
    if(ret != PA_OK) {
        cerr << "PulseAudio stream connect upload failed" << endl;
        exit(EXIT_FAILURE);
    }
    
    // feed some data into the stream, skip the wav header
    size_t cur = wav_hdr_size;
    for(size_t i=0;;) {
      
		if(PA_STREAM_READY==pa_stream_get_state(paStream)) {
            const size_t writableSize = pa_stream_writable_size(paStream);
            const size_t sizeRemain = size - cur;
            const size_t writeSize = sizeRemain<writableSize ? sizeRemain : writableSize;
            cout << "Ready: Writable " 
                 << writableSize << " Remain " 
                 << sizeRemain << " " << " Size " 
                 << writeSize << endl;
            if(writeSize>0) {
                pa_stream_write(paStream,&data[cur],writeSize,NULL,0,PA_SEEK_RELATIVE);
                cur += writeSize;
            }
		}
        else  {
            cout << "Timeout..."<< i << endl;
            i++;
        }
        
        pa_mainloop_iterate(paMainLoop,0,NULL);
        
        if(size<=cur ||
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
    
    
    ret = pa_stream_finish_upload(paStream);
        
    if(ret != PA_OK) {
        cerr << "PulseAudio stream finish upload failed" << endl;
        exit(EXIT_FAILURE);
    }
   
    
    pa_mainloop_iterate(paMainLoop,0,NULL);
    
    return paStream;
}

pa_sample_spec Sound::getFormat(const unsigned char *data, size_t size)
{
    // we decode the wav header (44 bytes long)
    SF_INFO sfinfo;
    sf_userdata user;
    user.curPos = 0;
    user.data = data;
    user.filelen = size;
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
               
    pa_sample_spec ss = {
        format,
        (uint32_t)sfinfo.samplerate, 
        (uint8_t)sfinfo.channels 
    };
    
    return ss;
}

// callbacks for reading from memory

sf_count_t Sound::sf_vio_tell(void *user_data)
{
    return static_cast<sf_userdata*>(user_data)->curPos;
}

void Sound::pa_context_success_cb(pa_context *c, int success, void *userdata)
{
    cout << "context success " << success << endl; 
}

void Sound::pa_stream_success_cb(pa_stream *s, int success, void *userdata)
{
    cout << "stream success " << success << endl; 
}

void Sound::pa_sample_info_cb(pa_context *c, const pa_sample_info *i, int eol, void *userdata)
{
    cout << "info: " << i->name << " vol " << i->volume.values << endl;
    cout << "comp " << pa_cvolume_compatible_with_channel_map(&i->volume, &i->channel_map) << endl;
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



