#ifndef SOUND_H
#define SOUND_H

#include <time.h>
#include <pulse/mainloop.h>
#include <pulse/context.h>
#include <pulse/stream.h>
#include <sndfile.h>

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
    
    void Play();
    
private:
    Sound ();
    ~Sound ();  
    // Singleton: Stop the compiler generating methods of copy the object
    Sound(Sound const& copy);            // Not Implemented
    Sound& operator=(Sound const& copy); // Not Implemented

    pa_mainloop *paMainLoop;
    pa_context *paContext;
    pa_stream *paStream;
    
    typedef struct sf_userdata {
        size_t filelen;
        size_t curPos;
        const unsigned char* data;
    } sf_userdata;
    
    static sf_count_t sf_vio_get_filelen(void *user_data);
    static sf_count_t sf_vio_seek(sf_count_t offset, int whence, void *user_data);
    static sf_count_t sf_vio_read(void *ptr, sf_count_t count, void *user_data) ;
    static sf_count_t sf_vio_write(const void *ptr, sf_count_t count, void *user_data) ;
    static sf_count_t sf_vio_tell(void *user_data) ;
    
};

#endif // SOUND_H
