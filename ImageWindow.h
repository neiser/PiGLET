#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H

#include "Window.h"

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
    
    int Init() { return 0; }
};

#endif // IMAGEWINDOW_H
