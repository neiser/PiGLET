#include <iostream>
#include <unistd.h>
#include <errno.h>
#include "ImageWindow.h"
#include "TextRenderer.h"
#include "ConfigManager.h"

using namespace std;

ImageWindow::ImageWindow( WindowManager* owner, const string& title, const float xscale, const float yscale ):
    Window(owner, title, xscale, yscale),
    _url("http://auge.physik.uni-mainz.de/record/current.jpg"),
    _delay(1e3),
    _label(this, -.95, .82, .95, .98),
    _image_ok(false)
{
    cout << "ImageWindow ctor" << endl;
    _label.SetText(title);    
    
    ConfigManager::I().addCmd(title+"_SetDelay", BIND_MEM_CB(&ImageWindow::callbackSetDelay, this));
    ConfigManager::I().addCmd(title+"_SetURL", BIND_MEM_CB(&ImageWindow::callbackSetURL, this));
    
    
    // stuff for the image loading
    MagickWandGenesis(); // just once somewhere...also in TextRenderer
    _mw = NewMagickWand();   
    pthread_mutex_init(&_mutex, NULL);
    // start loading already now
    pthread_create(&_thread, 0, &ImageWindow::start_thread, this);
}

ImageWindow::~ImageWindow()
{
    // wait until thread has finished loading
    pthread_join(_thread, NULL);
    pthread_mutex_destroy(&_mutex);
    DestroyMagickWand(_mw);
    cout << "ImageWindow dtor" << endl;
}

void ImageWindow::SetURL(const string &url)
{
    pthread_mutex_lock(&_mutex);
    _url = url;
    pthread_mutex_unlock(&_mutex);
}

int ImageWindow::callbackSetDelay(const string &arg)
{
    // maybe delay should be mutexed (see then do_work()), 
    // but this can take looong
    int d = atoi(arg.c_str());
    // 20ms should be minimum
    if(d>20) {
        _delay = d;
        return 0;
    }
    else {  
        return 1;
    }
}

int ImageWindow::callbackSetURL(const string &arg)
{
    SetURL(arg);
    return 0; // always works, but nobody knows if an image gets displayed
}

void ImageWindow::Draw()
{
    int state = pthread_tryjoin_np(_thread,NULL);
    if(state==0) {
        if(_image_ok) {
            TextRenderer::Image2Texture(_mw, _tex);
            DestroyMagickWand(_mw);
            _mw = NewMagickWand();
            //cout << "Image loaded..." << endl;
            _image_ok = false;
        }
        else {
            _label.SetColor(dMajorAlarm); // show that something is wrong
        }
        pthread_create(&_thread, 0, &ImageWindow::start_thread, this);
        
    }
    else if(state != EBUSY) {
        //cout << "Something wrong with image thread..." << endl;
    }               
    
    glPushMatrix();
    glScalef(.95,.83,.1);
    glTranslatef(.0,-.07,.0);    
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    _tex.Activate();
    
    Rectangle::unit.Draw( GL_TRIANGLE_FAN );
    
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();
    
    _label.Draw();
}

void ImageWindow::do_work()
{
    pthread_mutex_lock(&_mutex);
    _image_ok = MagickReadImage(_mw, _url.c_str());   
    pthread_mutex_unlock(&_mutex);
    usleep(1e3*_delay);    
}


