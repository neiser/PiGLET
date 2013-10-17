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
        //cout << "New Value: x="<< std::setprecision(12) <<t<<" y="<<y << endl;
        //graph.SetNow(t);
        vec2_t n;
        n.x = t;
        n.y = y;
        graph.AddToBlockList(n);
        break;
    }
}

PlotWindow::PlotWindow( const std::string& pvname,
                        const std::string& xlabel,
                        const std::string& ylabel,
                        const float xscale,
                        const float yscale):
    Window(pvname,xscale,yscale),
    _xlabel(xlabel),
    _ylabel(ylabel),
    _pvname(pvname),
    WindowArea( dBackColor, dWindowBorderColor),
    graph(this, 10),
    text(this, -.95,0.82,.95,.98),
    frame(0)
{
    text.SetText(pvname);
    // don't forget to call Init()
    // which also checks if the pvname is actually valid
}

PlotWindow::~PlotWindow() {
    cout << "Plotwindow dtor" << endl;
    if(_initialized) {
        Epics::I().removePV(_pvname);
    }
} 

void PlotWindow::Draw(){
        
    // Window border
    WindowArea.Draw();
    
    graph.Draw();
    
    text.Draw();
        
    graph.SetNow(Epics::I().GetCurrentTime());
    
    ++frame;
}

int PlotWindow::Init()
{
    int ret = Epics::I().addPV(_pvname, BIND_MEM_CB(&PlotWindow::callback_epics, this));
    _initialized = ret == 0;
    return ret;
}

void PlotWindow::Update() 
{ 
    graph.UpdateTicks(); 
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
