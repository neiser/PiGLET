#ifndef WINDOW_H
#define WINDOW_H

#include <ostream>
#include <string>

#include "Structs.h"
#include "BlockBuffer.h"
#include "Widget.h"
#include "NumberLabel.h"
#include "TextLabel.h"
#include "SimpleGraph.h"
#include "Epics.h"

class WindowManager;

class Window {
    
private:
    WindowManager* _owner;
    std::string _name; // unique window name
    float _x_pixels;
    float _y_pixels;   
    
    int callback_remove_window(const std::string& arg);
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
    virtual int Init() = 0;

};

std::ostream& operator<<( std::ostream& stream, const Window& win );




class ImageWindow: public Window {
private:
    Texture _image;
    std::string _url;

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
};

#endif // INTERVAL_H
