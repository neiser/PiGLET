#ifndef WINDOW_H
#define WINDOW_H

#include <ostream>
#include <string>

#include "Structs.h"
#include "TextRenderer.h"
#include "BlockBuffer.h"
#include "Widget.h"
#include "NumberLabel.h"
#include "SimpleGraph.h"
class Window {
private:
    float _x_pixles;
    float _y_pixels;
    std::string _title;

public:

    Window( const std::string& title, const float xscale = 1, const float yscale = 1): _title(title) , _x_pixles(xscale), _y_pixels(yscale) {}

    virtual ~Window(){}

    const float& XPixels() const { return _x_pixles; }
    const float& YPixels() const { return _y_pixels; }
    float& XPixels() { return _x_pixles; }
    float& YPixels() { return _y_pixels; }

    virtual void Update() = 0;

    virtual void Draw() = 0;

};

class PlotWindow: public Window{
private:
    std::string _xlabel;
    std::string _ylabel;

    UnitBorderBox WindowArea;
    UnitBorderBox PlotArea;

    SimpleGraph graph;

    TextLabel text;

    int frame;          //for debug

public:
    const std::string& Xlabel() const { return _xlabel; }
    const std::string& Ylabel() const { return _ylabel; }
    std::string& Xlabel() { return _xlabel; }
    std::string& Ylabel() { return _ylabel; }

    PlotWindow( const std::string& title,
                const std::string& xlabel = "Always label your axes",
                const std::string& ylabel = "Alawys label your axes",
                const float xscale = 1,
                const float yscale = 1);

    virtual ~PlotWindow(){}


    virtual void Update() { graph.UpdateTicks(); }
    virtual void Draw();
};

std::ostream& operator<<( std::ostream& stream, const Window& win );
std::ostream& operator<<( std::ostream& stream, const PlotWindow& win );

#endif // INTERVAL_H
