#include "Window.h"
#include <sstream>
#include <cmath>
#include "Widget.h"
#include "GLTools.h"

PlotWindow::PlotWindow( const std::string& title,
            const std::string &xlabel,
            const std::string &ylabel,
            const float xscale,
            const float yscale):
    Window(title,xscale,yscale),
    _xlabel(xlabel),
    _ylabel(ylabel),
    WindowArea( dBackColor, dWindowBorderColor),
    graph(this, 10),
    text(this, -.95,0.82,.95,.98),
    frame(0)
{
    text.SetText(title);
}

void PlotWindow::Draw(){

    // Window border
    WindowArea.Draw();

    graph.Draw();

    text.Draw();


    // feed some data - only for testing
    if( frame %10 == 0 ) {
        vec2_t n;
        n.x = frame/100.0;
        n.y = sin(3.14157*frame/1000.0);
    	graph.AddToBlockList(n);

    } else {
        graph.SetNow(frame/100.0);
    }
	++frame;
}

std::ostream& operator<<( std::ostream& stream, const Window& win ) {
    stream << "[ " << win.XPixels() << " x " << win.YPixels() << " ]";
	return stream;
}


std::ostream& operator<<( std::ostream& stream, const PlotWindow& win ) {
    stream << "[ " << win.XPixels() << " x " << win.YPixels() << " ]: " << win.Xlabel() << " vs. " << win.Ylabel();
	return stream;
}
