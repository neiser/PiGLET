#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H


#include <pthread.h>
#include <wand/magick_wand.h>

#include "Window.h"
#include "TextLabel.h"

class ImageWindow: public Window {
private:
      
    std::string _url;    
    int _delay;
    TextLabel _label;
    
    Texture _tex;
    bool _image_ok;
    pthread_t _thread;    
    pthread_mutex_t _mutex;    
    MagickWand* _mw;
    
    
    // This is the static class function that serves as a C style function pointer
    // for the pthread_create call
    static void* start_thread(void *obj)
    {
        //All we do here is call the do_work() function
        reinterpret_cast<ImageWindow*>(obj)->do_work();
        return NULL;
    }

    void do_work();
    
    std::string callbackSetDelay( const std::string& arg );
    std::string callbackSetURL( const std::string& arg );
    
public:
    ImageWindow( 
            WindowManager* owner,
            const std::string& title, 
            const float xscale = 1, const float yscale = 1);
    virtual ~ImageWindow();

    void SetURL( const std::string& url);
    const std::string& GetURL() const { return _url; }
    void Update() {}
    void Draw();
    
    int Init();
};

#endif // IMAGEWINDOW_H
