#ifndef SOUND_H
#define SOUND_H

#include <time.h>
#include <pulse/mainloop.h>
#include <pulse/context.h>
#include <pulse/stream.h>

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
    
};

#endif // SOUND_H
