#ifndef SOUND_H
#define SOUND_H

#include <string>
#include <map>
#include <time.h>
#include <pulse/mainloop.h>
#include <pulse/context.h>
#include <pulse/stream.h>
#include <pulse/introspect.h>
#include <sndfile.h>
#include <pthread.h>

#include "Epics.h"
#include "StopWatch.h"

class Sound
{
public:
    // Implement a singleton
    static Sound& I() {
        // Returns the only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static Sound instance;
        return instance;
    }
    
    bool Play(const std::string& name);
    
private:
    Sound ();
    ~Sound ();  
    // Singleton: Stop the compiler generating methods of copy the object
    Sound(Sound const& copy);            // Not Implemented
    Sound& operator=(Sound const& copy); // Not Implemented
   
    pthread_t _thread;
    volatile bool _running;
    pthread_mutex_t _mutex;
    pthread_cond_t _signal;
    
    StopWatch _muted;
    double _muted_for;
    const std::string _pvname;
    void ProcessEpicsData(const Epics::DataItem *i);
    
    // This is the static class function that serves as a C style function pointer
    // for the pthread_create call
    static void* start_thread(void *obj)
    {
        //All we do here is call the do_work() function
        reinterpret_cast<Sound*>(obj)->do_work();
        return NULL;
    }

    void do_work();
    
    pa_mainloop *paMainLoop;
    pa_context *paContext;
    //pa_stream *paStream;
    
    const static size_t max_timeouts = 1000;
    
    typedef struct wav_item_t {
        size_t filelen;
        size_t curPos;
        const unsigned char* data;
    } wav_item_t;
    
    wav_item_t* _cur_item;
    
    std::map<std::string, wav_item_t*> wavs;    
    
    void SetupWavItem(const std::string &name, const unsigned char* data, size_t size);
    void PlayWavItem(wav_item_t* item);
    static pa_sample_spec getFormat(wav_item_t* item);
    static sf_count_t sf_vio_get_filelen(void *user_data);
    static sf_count_t sf_vio_seek(sf_count_t offset, int whence, void *user_data);
    static sf_count_t sf_vio_read(void *ptr, sf_count_t count, void *user_data);
    static sf_count_t sf_vio_tell(void *user_data);
    
    static void pa_context_success_cb(pa_context *c, int success, void *userdata);
    static void pa_stream_success_cb (pa_stream* s, int success, void *userdata);
    static void pa_sample_info_cb(pa_context *c, const pa_sample_info *i, int eol, void *userdata);
    
    
};

#endif // SOUND_H
