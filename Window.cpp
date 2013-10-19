#include "Window.h"
#include <sstream>
#include <cmath>
#include "Widget.h"
#include "GLTools.h"
#include <iostream>
#include <iomanip>
#include "TextRenderer.h"
#include "ConfigManager.h"
#include "WindowManager.h"

using namespace std;




ImageWindow::ImageWindow( WindowManager* owner, const std::string& title, const float xscale, const float yscale ):
    Window(owner, title, xscale, yscale),
    _url("")
{
}

void ImageWindow::SetURL(const std::string &url)
{
    _url = url;
    UpdateImage();
}

void ImageWindow::UpdateImage()
{
    TextRenderer::I().LoadImage(_image, _url);
}

void ImageWindow::Draw()
{
    glPushMatrix();
    glScalef(.9,.9,.9);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    _image.Activate();
    
    Rectangle::unit.Draw( GL_TRIANGLE_FAN );
    
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();
}


int Window::callback_remove_window(const string &arg)
{
    return _owner->RemoveWindow(_name);    
}

Window::Window(WindowManager *owner, const string &name, const float xscale, const float yscale) : 
    _owner(owner),
    _name(name), 
    _x_pixels(xscale), 
    _y_pixels(yscale) {
    ConfigManager::I().addCmd(name+"_Remove",BIND_MEM_CB(&Window::callback_remove_window,this));
}

Window::~Window()
{
    ConfigManager::I().removeCmd(_name+"_Remove");
}

std::ostream& operator<<( std::ostream& stream, const Window& win ) {
    stream << "[ " << win.XPixels() << " x " << win.YPixels() << " ]";
    return stream;
}
