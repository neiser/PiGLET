

#include <iostream>
#include <pulse/error.h>
#include <pulse/scache.h>

#include <sndfile.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "Sound.h"
#include "Structs.h"

extern "C" {
#include "wavfiles.h"
}

using namespace std;

bool Sound::Play(const string& name)
{
    if(wavs.count(name)==0)
        return false;
        
    _muted.Stop();
    if(_muted.TimeElapsed()<_muted_for)
        return false;
    
    int state = pthread_mutex_trylock(&_mutex);
    if(state==0) {
        // mutex was free and now locked, 
        // so we can play
        _cur_item = wavs[name];
        // signal the thread to play it
        pthread_cond_signal(&_signal);
        // don't forget to unlock
        pthread_mutex_unlock(&_mutex);
        
        return true;
    }
    else if(state != EBUSY) {
        cerr << "Something wrong with play thread..." << endl;
    }      
    return false;
}

Sound::Sound() : _running(true), 
    _pvname("GEN:ONLINEDISPLAYS:MUTE"), // the PV name to mute all displays for a specific time
    _cur_item(NULL)
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
    
    // setup the available wavs, this is somewhat manual...
    
    SetupWavItem("alert", sound_alert_wav, sound_alert_wav_size);
    SetupWavItem("warning", sound_warning_wav, sound_warning_wav_size);
    
    // thread items
    pthread_mutex_init(&_mutex, NULL);
    pthread_cond_init (&_signal, NULL);    
    
    pthread_create(&_thread, 0, &Sound::start_thread, this);
    
    // register global MUTE PV for all displays
    // request that ProcessEpicsData gets automatically called for each new DataItem
    _muted.Start();
    Epics::I().addPV(_pvname, BIND_MEM_CB(&Sound::ProcessEpicsData, this), true);
}

void Sound::SetupWavItem(const string& name, const unsigned char *data, size_t size)
{
    wav_item_t* item = new wav_item_t;
    item->curPos = 0;
    item->data = data;
    item->filelen = size;
    wavs[name] = item;
}

void Sound::ProcessEpicsData(const Epics::DataItem *i)
{
    _muted.Start();
    
    switch (i->type) {
        
    case Epics::Connected:
        _muted_for = 0;
        break;
    case Epics::Disconnected:
        _muted_for = 0;
        break;
        
    case Epics::NewValue: {
        vec2_t* d = (vec2_t*)i->data;
        _muted_for = d->y>0 ? d->y : 0;
        break;               
    }
    default: 
        break;
    }  
}

Sound::~Sound()
{    
    // we stop the while loop
    pthread_mutex_lock(&_mutex);
    _running = false;
    pthread_cond_signal(&_signal);
    pthread_mutex_unlock(&_mutex);
    
    // wait until thread has really finished 
    pthread_join(_thread, NULL);
    pthread_cond_destroy(&_signal);
    pthread_mutex_destroy(&_mutex);    
    
    pa_context_disconnect(paContext);
	pa_context_unref(paContext);
	pa_mainloop_free(paMainLoop);  
    for (map<string, wav_item_t*>::iterator it = wavs.begin(); it != wavs.end(); ++it ) {
        delete it->second;
    }
    
    Epics::I().removePV(_pvname);
}

void Sound::do_work()
{
    pthread_mutex_lock(&_mutex);
    while(_running) {
        if(_cur_item != NULL) {
            PlayWavItem(_cur_item);        
            _cur_item = NULL;
        }
        pthread_cond_wait(&_signal, &_mutex);
    }
    pthread_mutex_unlock(&_mutex);      
    pthread_exit(0);
}

void Sound::PlayWavItem(wav_item_t *item)
{
    pa_sample_spec ss = getFormat(item);  
    
    pa_stream* paStream = pa_stream_new(paContext,"PiGLETStream",&ss, NULL);
   
	if(NULL==paStream) {
		cerr << "Cannot create PulseAudio stream" << endl;
	}        
    
    // don't forget to set the volume (otherwise Raspberry Pi stays silent)
    pa_cvolume cv;    
    int ret = pa_stream_connect_playback(paStream,NULL,NULL,(pa_stream_flags_t)0,pa_cvolume_reset(&cv, ss.channels),NULL);
    
    if(ret != PA_OK) {
        cerr << "PulseAudio stream connect upload failed" << endl;
        exit(EXIT_FAILURE);
    }
    
    // feed some data into the stream, skip the wav header
    static const size_t wav_hdr_size = 44;
    size_t cur = wav_hdr_size;
    for(size_t i=0;;) {
      
		if(PA_STREAM_READY==pa_stream_get_state(paStream)) {
            const size_t writableSize = pa_stream_writable_size(paStream);
            const size_t sizeRemain = item->filelen - cur;
            const size_t writeSize = sizeRemain<writableSize ? sizeRemain : writableSize;
//            cout << "Ready: Writable " 
//                 << writableSize << " Remain " 
//                 << sizeRemain << " " << " Size " 
//                 << writeSize << endl;
            if(writeSize>0) {
                pa_stream_write(paStream,&item->data[cur],writeSize,NULL,0,PA_SEEK_RELATIVE);
                cur += writeSize;
            }
		}
        else  {
            //cout << "Timeout..."<< i << endl;
            i++;
        }
        
        pa_mainloop_iterate(paMainLoop,0,NULL);
        
        if(item->filelen<=cur ||
		   0<=pa_stream_get_underflow_index(paStream))
		{
            //cout << "Underflow (maybe playback done)" << endl;
            break;
		}
               
        if(i==max_timeouts) {
            cerr << "Stream could not be written (too many tries)" << endl;
            exit(EXIT_FAILURE);
        }
        usleep(10000); // wait 10 ms
    }   
       
    // ensure we're done playing
    pa_operation *o = pa_stream_drain(paStream, NULL, NULL);
    while(pa_operation_get_state(o) == PA_OPERATION_RUNNING) {
        pa_mainloop_iterate(paMainLoop,0,NULL);
        usleep(1000);
    }
   
    pa_stream_disconnect(paStream);
	pa_stream_unref(paStream);    
}

pa_sample_spec Sound::getFormat(wav_item_t* item)
{
    // we decode the wav header (44 bytes long)
    item->curPos = 0; // ensure we start at zero        
    SF_VIRTUAL_IO sf_vio;
    sf_vio.get_filelen = &Sound::sf_vio_get_filelen;
    sf_vio.read = &Sound::sf_vio_read;
    sf_vio.tell = &Sound::sf_vio_tell;
    sf_vio.seek = &Sound::sf_vio_seek;
    SF_INFO sfinfo;
    SNDFILE* sf = sf_open_virtual(&sf_vio, SFM_READ, &sfinfo, item);
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
    return static_cast<wav_item_t*>(user_data)->curPos;
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
    wav_item_t* data = static_cast<wav_item_t*>(user_data);
    memcpy(ptr, data->data+data->curPos, count);
    data->curPos += count;
    return count;
}

sf_count_t Sound::sf_vio_seek(sf_count_t offset, int whence, void *user_data)
{
    wav_item_t* data = static_cast<wav_item_t*>(user_data);
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
    return static_cast<wav_item_t*>(user_data)->filelen;
}


