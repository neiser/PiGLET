#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H


#include <pthread.h>
#include <wand/magick_wand.h>

#include "Window.h"
#include "TextLabel.h"
#include "TextRenderer.h"

class ImageWindow: public Window {
private:
      
    std::string _url;    
    long _delay;
    TextLabel _label;

    static const Color color;
    
    Texture _tex;
    TextRenderer _render;
    bool _image_ok;
    pthread_t _thread;
    volatile bool _running;
    pthread_mutex_t _mutex_running;  // locked if do_work is running (unlocked if thread has some data)
    pthread_mutex_t _mutex_working;  // locked if do_work is either loading an image
    pthread_cond_t _signal;        // indicates that the Draw routine has used the loaded image
    pthread_cond_t _signal_delay; // sleeping _delay can be aborted
   
    size_t  _crop_w, _crop_h, _crop_x, _crop_y;
    size_t  _crosshair_x, _crosshair_y, _crosshair_size;
    size_t  _rect_x, _rect_y, _rect_size;
    
    
    // This is the static class function that serves as a C style function pointer
    // for the pthread_create call
    static void* start_thread(void *obj)
    {
        //All we do here is call the do_work() function
        reinterpret_cast<ImageWindow*>(obj)->do_work();
        return NULL;
    }

    void do_work();

    bool ApplyTexture(int state);
    
    std::string callbackSetDelay( const std::string& arg );
    std::string callbackSetURL( const std::string& arg );
    std::string callbackSetCrop( const std::string& arg );
    std::string callbackSetCrosshair( const std::string& arg );
    std::string callbackSetRectangle( const std::string& arg );
    
public:
    ImageWindow( 
            WindowManager* owner,
            const std::string& title, 
            const float xscale = 1, const float yscale = 1);
    virtual ~ImageWindow();

    void SetURL(const std::string& url);
    
    void Update() {}
    void Draw();
    
    int Init();
};

#endif // IMAGEWINDOW_H
