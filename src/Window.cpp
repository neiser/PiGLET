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

Window::Window(WindowManager *owner, const string &name, const float xscale, const float yscale) : 
    _owner(owner),
    _name(name), 
    _x_pixels(xscale), 
    _y_pixels(yscale) {
    
   
}

Window::~Window()
{
    ConfigManager::I().removeCmd(_name+"_Remove");
}

int Window::Init()
{
    // windows can remove themselves
    ConfigManager::I().addCmd(Name()+"_Remove",BIND_MEM_CB(&Window::callbackRemoveWindow,this));
    return 0;
}

string Window::callbackRemoveWindow(const string &arg)
{
    return _owner->RemoveWindow(_name)==0 ? "" : "Window not found.";    
}


std::ostream& operator<<( std::ostream& stream, const Window& win ) {
    stream << "[ " << win.XPixels() << " x " << win.YPixels() << " ]";
    return stream;
}
