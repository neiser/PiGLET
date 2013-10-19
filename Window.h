#ifndef WINDOW_H
#define WINDOW_H

#include <ostream>
#include <string>

#include "Structs.h"
#include "BlockBuffer.h"
#include "Widget.h"
#include "NumberLabel.h"

class WindowManager;

class Window {
    
private:
    WindowManager* _owner; 
    std::string _name; // unique window name
    float _x_pixels;
    float _y_pixels;   
    
    int callbackRemoveWindow(const std::string& arg);       
public:

    Window( WindowManager* owner,
            const std::string& name, 
            const float xscale = 1, 
            const float yscale = 1);

    virtual ~Window();
      
    
    const float& XPixels() const { return _x_pixels; }
    const float& YPixels() const { return _y_pixels; }  
    float& XPixels()  { return _x_pixels; }
    float& YPixels()  { return _y_pixels; }  
    
    const std::string& Name() const { return _name; }
    
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual int Init();

};

std::ostream& operator<<( std::ostream& stream, const Window& win );

#endif // WINDOW_H
