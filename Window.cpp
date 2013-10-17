#include "Window.h"
#include <sstream>
#include <cmath>
#include "Widget.h"
#include "GLTools.h"
#include <iostream>
#include <iomanip>
#include "TextRenderer.h"

using namespace std;

void PlotWindow::callback_epics(const Epics::CallbackMode &m, const double &t, const double &y)
{
    
    switch(m) {
    case Epics::Connected:
        cout << "Connected to EPICS!" << endl;
        break;
    case Epics::Disconnected:
        cout << "Disconnected from EPICS!" << endl;
        break;
    case Epics::NewValue:
        cout << "New Value: x="<< std::setprecision(12) <<t<<" y="<<y << endl;
        //graph.SetNow(t);
        vec2_t n;
        n.x = t;
        n.y = y;
        graph.AddToBlockList(n);
        break;
    }
}

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
    Epics::I().addPV(title, BIND_MEM_CB(&PlotWindow::callback_epics, this));
}

void PlotWindow::Draw(){
    
    // Window border
    WindowArea.Draw();
    
    graph.Draw();
    
    text.Draw();
    
    
    // feed some data - only for testing
    //    if( frame %10 == 0 ) {
    //        vec2_t n;
    //        n.x = frame/100.0;
    //        n.y = sin(3.14157*frame/1000.0);
    //    	graph.AddToBlockList(n);
    
    //    } else {
    //graph.SetNow(frame/100.0);
    //    }
    graph.SetNow(Epics::I().GetCurrent());
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


ImageWindow::ImageWindow( const std::string& title, const float xscale, const float yscale ):
    Window(title, xscale, yscale),
    _url("")
{
}

void ImageWindow::SetURL(const std::string &url)
{
    _url = url;
    UpdateImage();
}

void ImageWindow::UpdateImage()
{
    TextRenderer::I().LoadImage(_image, _url);
}

void ImageWindow::Draw()
{
    glPushMatrix();
    glScalef(.9,.9,.9);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    _image.Activate();

    Rectangle::unit.Draw( GL_TRIANGLE_FAN );

    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();
}
