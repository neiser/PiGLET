#include <iostream>
#include <sstream>
#include <errno.h>
#include "ImageWindow.h"
#include "TextRenderer.h"
#include "ConfigManager.h"

using namespace std;

const Color ImageWindow::color(kWhite);

ImageWindow::ImageWindow( WindowManager* owner, const string& title, const float xscale, const float yscale ):
    Window(owner, title, xscale, yscale),
    _url(""),
    _delay(0), // see Init() for the correct default delay
    _label(this, -.95, .82, .95, .98),
    _image_ok(false),
    _running(true),
    _crop_w(0), _crop_h(0), _crop_x(0), _crop_y(0),
    _crosshair_x(0), _crosshair_y(0), _crosshair_size(0),
    _rect_x(0), _rect_y(0), _rect_size(0)
{
    cout << "ImageWindow ctor" << endl;
    _label.SetText(title);    
    _label.SetColor(dInvalidAlarm);
    
    // thread items
    pthread_mutex_init(&_mutex_running, NULL);
    pthread_mutex_init(&_mutex_working, NULL);    
    pthread_cond_init (&_signal, NULL);
    pthread_cond_init (&_signal_delay, NULL);    
}

int ImageWindow::Init() {
    // start loading already now, and get the first result 
    // immediately (default delay is zero timespec!)
    pthread_create(&_thread, 0, &ImageWindow::start_thread, this);
    if(ApplyTexture(pthread_mutex_lock(&_mutex_running))) {
        // by default, we update the image every second
        _delay = 1000;
        pthread_mutex_unlock(&_mutex_running);
    }
    
    ConfigManager::I().addCmd(Name()+"_Delay", BIND_MEM_CB(&ImageWindow::callbackSetDelay, this));
    ConfigManager::I().addCmd(Name()+"_URL", BIND_MEM_CB(&ImageWindow::callbackSetURL, this));
    ConfigManager::I().addCmd(Name()+"_Crop", BIND_MEM_CB(&ImageWindow::callbackSetCrop, this));
    ConfigManager::I().addCmd(Name()+"_Crosshair", BIND_MEM_CB(&ImageWindow::callbackSetCrosshair, this));    
    ConfigManager::I().addCmd(Name()+"_Rectangle", BIND_MEM_CB(&ImageWindow::callbackSetRectangle, this));    
    
    return Window::Init();
}

ImageWindow::~ImageWindow()
{
    ConfigManager::I().removeCmd(Name()+"_Delay");
    ConfigManager::I().removeCmd(Name()+"_URL");
    ConfigManager::I().removeCmd(Name()+"_Crop");
    ConfigManager::I().removeCmd(Name()+"_Crosshair");
    ConfigManager::I().removeCmd(Name()+"_Rectangle");
    
    
   
    // at this point, we don't know in which state the 
    // thread is (loading an image or sleeping, or waiting in between)
    
    // we negate a possible long delay by signaling it
    pthread_mutex_lock(&_mutex_working);
    pthread_cond_signal(&_signal_delay);
    pthread_mutex_unlock(&_mutex_working);
    
    // then we stop the while loop
    pthread_mutex_lock(&_mutex_running);
    _running = false;
    pthread_cond_signal(&_signal);
    pthread_mutex_unlock(&_mutex_running);    
    
    // wait until thread has really finished    
    pthread_join(_thread, NULL);
    pthread_cond_destroy(&_signal);
    pthread_cond_destroy(&_signal_delay);    
    pthread_mutex_destroy(&_mutex_working);    
    pthread_mutex_destroy(&_mutex_running);
    
    cout << "ImageWindow dtor" << endl;
}

void ImageWindow::SetURL(const string &url)
{
    // wait until thread has finished working
    pthread_mutex_lock(&_mutex_working);
    _url = url;
    // negate the delay
    pthread_cond_signal(&_signal_delay);            
    pthread_mutex_unlock(&_mutex_working);
}



string ImageWindow::callbackSetDelay(const string &arg)
{
    int d = atoi(arg.c_str());
    // 20ms should be minimum
    if(d>20) {
        pthread_mutex_lock(&_mutex_working);
        _delay = d;        
        // negate the delay
        pthread_cond_signal(&_signal_delay);        
        pthread_mutex_unlock(&_mutex_working);
        return "";
    }
    else {  
        return "Delay shorter than 20 ms";
    }
}

string ImageWindow::callbackSetURL(const string &arg)
{
    SetURL(arg);
    return ""; // always works, but nobody knows if an image gets displayed :)
}

string ImageWindow::callbackSetCrop(const string &arg)
{
    stringstream ss(arg);
    size_t w, h, x, y;
    if(!(ss >> w))
        return "First value (w) not a integer";
    if(!(ss >> h))
        return "Second value (h) not a integer";
    if(!(ss >> x))
        return "Third value (x) not a integer";
    if(!(ss >> y))
        return "Fourth value (y) not a integer";
    
    // wait until thread has finished working
    pthread_mutex_lock(&_mutex_working);
    _crop_w = w;
    _crop_h = h;
    _crop_x = x;
    _crop_y = y;
    // negate the delay
    pthread_cond_signal(&_signal_delay);            
    pthread_mutex_unlock(&_mutex_working);    
    return ""; // success
}

string ImageWindow::callbackSetCrosshair(const string &arg)
{    
    stringstream ss(arg);
    size_t x, y, size;
    if(!(ss >> x))
        return "First value (x) not a integer";
    if(!(ss >> y))
        return "Second value (y) not a integer";
    if(!(ss >> size))
        return "Third value (size) not a integer";
    
    // wait until thread has finished working
    pthread_mutex_lock(&_mutex_working);
    _crosshair_x = x;
    _crosshair_y = y;
    _crosshair_size = size;
    // negate the delay
    pthread_cond_signal(&_signal_delay);            
    pthread_mutex_unlock(&_mutex_working);    
    return ""; // success
}

string ImageWindow::callbackSetRectangle(const string &arg)
{    
    stringstream ss(arg);
    size_t x, y, size;
    if(!(ss >> x))
        return "First value (x) not a integer";
    if(!(ss >> y))
        return "Second value (y) not a integer";
    if(!(ss >> size))
        return "Third value (size) not a integer";
    
    // wait until thread has finished working
    pthread_mutex_lock(&_mutex_working);
    _rect_x = x;
    _rect_y = y;
    _rect_size = size;
    // negate the delay
    pthread_cond_signal(&_signal_delay);            
    pthread_mutex_unlock(&_mutex_working);    
    return ""; // success
}

bool ImageWindow::ApplyTexture(int state)
{
    if(state==0) {
        // mutex was free and now locked, so data is available
        if(_image_ok) {
            _render.Mw2Texture(_tex);
            //cout << "Image loaded..." << endl;
            _image_ok = false;
            _label.SetColor(dTextColor);
        }
        else {
            _label.SetColor(dMajorAlarm); // show that something is wrong
            _render.Text2Texture( _tex, "No Image");
        }

        pthread_cond_signal(&_signal);
        return true;
    }
    else if(state != EBUSY) {
        cerr << "Something wrong with image thread..." << endl;
    }      
    return false;
}

void ImageWindow::Draw()
{    
    if(ApplyTexture(pthread_mutex_trylock(&_mutex_running))) {
        pthread_mutex_unlock(&_mutex_running);
    }
             
    glPushMatrix();
    glScalef(.98,.98,.1);
    //glTranslatef(.0,-.07,.0);    

    color.Activate();
    _tex.Activate();

    const float winratio =XPixels() / YPixels();
    const float totalratio = _tex.GetAspectRatio() / winratio;
    
    if( totalratio >= 1.0f )
        glScalef(1.0f,1.0f/totalratio,1.0f);
    else
        glScalef(1.0f*totalratio,1.0f,1.0f);
    
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    Rectangle::unit.Draw( GL_TRIANGLE_FAN );

    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glLineWidth(1.0f);
    Rectangle::unit.Draw( GL_LINE_LOOP );


    glPopMatrix();
    
    _label.Draw();
}

void ImageWindow::do_work()
{
    pthread_mutex_lock(&_mutex_running);
    while(_running) {
        pthread_mutex_lock(&_mutex_working);
        _image_ok = _render.Image2Mw(_url, 
                                     _crop_w, _crop_h, _crop_x, _crop_y,
                                     _crosshair_x, _crosshair_y, _crosshair_size,
                                     _rect_x, _rect_y, _rect_size);
        // first we wait with a condition timed wait, 
        // serves as a usleep 
        // but can be cancelled via _signal_delay 
        // to get the data quicker
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts); // the current time
        long sec =_delay / 1000;
        long ms = _delay - sec*1e3;
        
        // perform the addition
        ts.tv_nsec+=ms*1e6;
        
        // adjust the time
        ts.tv_sec+=ts.tv_nsec/1e9 + sec;
        ts.tv_nsec=ts.tv_nsec%(long)1e9;    
        
        pthread_cond_timedwait(&_signal_delay, &_mutex_working, &ts);   
        pthread_mutex_unlock(&_mutex_working);       
        
        // then we wait that somebody has used the data
        pthread_cond_wait(&_signal, &_mutex_running);
    }
    pthread_mutex_unlock(&_mutex_running);      
    pthread_exit(0);
}



