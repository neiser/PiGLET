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
    PlotArea( dPlotBackground, dWindowBorderColor),
    graph(this, 10),
    text(this, -.95,0.82,.95,.98),
    frame(0)
{
    text.SetText(title);
}

void PlotWindow::Draw(){

    // Window border
    WindowArea.Draw();

	glPushMatrix();

        glScalef(0.8f,0.8f,0.8f);

        glEnable(GL_STENCIL_TEST);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);
        glStencilFunc(GL_NEVER, 1, 0xFF);
        glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);  // draw 1s on test fail (always)

        // draw stencil pattern
        glStencilMask(0xFF);
        glClear(GL_STENCIL_BUFFER_BIT);  // needs mask=0xFF

        PlotArea.Draw();

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);
        glStencilMask(0x00);
        // draw where stencil's value is 0
        glStencilFunc(GL_EQUAL, 0, 0xFF);
        /* (nothing to draw) */
        // draw only where stencil's value is 1
        glStencilFunc(GL_EQUAL, 1, 0xFF);

        PlotArea.Draw();

        glColor4f(1,1,1,1);
        graph.Draw();

        glDisable(GL_STENCIL_TEST);

	glPopMatrix();

	text.Draw( GL_LINE_LOOP );



    // feed some data
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
