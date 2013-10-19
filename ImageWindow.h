#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H

#include "Window.h"
#include <pthread.h>

class ImageWindow: public Window {
private:
    typedef struct DataItem {
        Texture* image;
        struct DataItem* prev;
    } DataItem;
    
    Texture _image;
    std::string _url;
    pthread_t _thread;
    
    static void deleteDataItem(DataItem* i);
    static void fillList(DataItem* head, std::vector<DataItem*>& list);
    
    // This is the static class function that serves as a C style function pointer
    // for the pthread_create call
    static void* start_thread(void *obj)
    {
        //All we do here is call the do_work() function
        reinterpret_cast<ImageWindow*>(obj)->do_work();
        return NULL;
    }

    void do_work();
    
public:
    ImageWindow( 
            WindowManager* owner,
            const std::string& title, 
            const float xscale = 1, const float yscale = 1);
    virtual ~ImageWindow() {}

    void SetURL( const std::string& url);
    const std::string& GetURL() const { return _url; }
    void UpdateImage();
    void Update() {}
    void Draw();
    
    int Init() { return 0; }
};

#endif // IMAGEWINDOW_H
