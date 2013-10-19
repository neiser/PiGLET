#include <iostream>
#include <unistd.h>
#include <errno.h>
#include "ImageWindow.h"
#include "TextRenderer.h"

using namespace std;

ImageWindow::ImageWindow( WindowManager* owner, const string& title, const float xscale, const float yscale ):
    Window(owner, title, xscale, yscale),
    _url("http://auge.physik.uni-mainz.de/record/current.jpg"),
    _image_ok(false)
{
    cout << "ImageWindow ctor" << endl;
    MagickWandGenesis();
    _mw = NewMagickWand();   
    // start loading already now
    pthread_create(&_thread, 0, &ImageWindow::start_thread, this);
}

ImageWindow::~ImageWindow()
{
    pthread_join(_thread, NULL);
    
    MagickWandTerminus();
    cout << "ImageWindow dtor" << endl;
}

void ImageWindow::SetURL(const string &url)
{
    _url = url;
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
            cout << "Image could not be loaded..." << endl;
        }
        pthread_create(&_thread, 0, &ImageWindow::start_thread, this);
        
    }
    else if(state != EBUSY) {
        cout << "Something wrong with image thread..." << endl;
    }               
    
    glPushMatrix();
    glScalef(.9,.9,.9);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    _tex.Activate();
    
    Rectangle::unit.Draw( GL_TRIANGLE_FAN );
    
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();
    
}

void ImageWindow::do_work()
{
    _image_ok = MagickReadImage(_mw, _url.c_str());   
    usleep(1e6);
}
