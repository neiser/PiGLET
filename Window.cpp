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
