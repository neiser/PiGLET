#ifndef WINDOW_H
#define WINDOW_H

#include <ostream>
#include <string>

#include "Structs.h"
#include "TextRenderer.h"
#include "BlockBuffer.h"


class Window {
private:
    int _width;
    int _height;
    std::string _title;

public:

    Window( const std::string& title, const int width = 1, const int height = 1): _title(title) , _width(width), _height(height) {}

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

    // Window Frame:
    Rectangle rect;

    PiGLPlot::BlockList graph;

    TextLabel text;

    int frame;          //for debug
    NumberLabel num;    //for debug

public:
    const std::string& Xlabel() const { return _xlabel; }
    const std::string& Ylabel() const { return _ylabel; }
    std::string& Xlabel() { return _xlabel; }
    std::string& Ylabel() { return _ylabel; }

    PlotWindow( const std::string& title,
                const std::string xlabel = "Always label your axes",
                const std::string ylabel = "Alawys label your axes",
                 const int width = 1, const int height = 1):
        Window(title,width,height),
        _xlabel(xlabel),
        _ylabel(ylabel),
        rect(-1,-1,1,1),
        graph(10.0),
        text(-.95,0.82,.95,.98),
        frame(0)
    {

        num.Init();
        text.SetText(title);
    }

    virtual void Draw();
};

std::ostream& operator<<( std::ostream& stream, const Window& win );
std::ostream& operator<<( std::ostream& stream, const PlotWindow& win );

#endif // INTERVAL_H
