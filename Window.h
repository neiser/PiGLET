#ifndef WINDOW_H
#define WINDOW_H

#include <ostream>
#include <string>

#include "Structs.h"
#include "TextRenderer.h"


class Window {
private:
    int _width;
    int _height;

public:

    Window( const int width, const int height): _width(width), _height(height) {}

    const int& Width() const { return _width; }
    const int& Height() const { return _height; }
    int& Width() { return _width; }
    int& Height() { return _height; }

    virtual void Draw() = 0;

};

class PlotWindow: public Window{
private:
    std::string _xlabel;
    std::string _ylabel;
    Rectangle rect;

public:
    const std::string& Xlabel() const { return _xlabel; }
    const std::string& Ylabel() const { return _ylabel; }
    std::string& Xlabel() { return _xlabel; }
    std::string& Ylabel() { return _ylabel; }
    PlotWindow( const int width, const int height,
                const std::string xlabel = "Always label your axes",
                const std::string ylabel = "Alawys label your Axes"):
        Window(width,height),
        _xlabel(xlabel),
        _ylabel(ylabel),
        rect(-1,-1,1,1)
    {}

    virtual void Draw();
};

std::ostream& operator<<( std::ostream& stream, const Window& win );
std::ostream& operator<<( std::ostream& stream, const PlotWindow& win );

#endif // INTERVAL_H
