#include "Window.h"
#include <sstream>
#include <cmath>
#include "Widget.h"
#include "GLTools.h"
#include <iostream>
#include <iomanip>
#include "TextRenderer.h"

using namespace std;

PlotWindow::PlotWindow( const std::string& pvname,
                        const std::string& xlabel,
                        const std::string& ylabel,
                        const float xscale,
                        const float yscale):
    Window(pvname,xscale,yscale),
    _pvname(pvname),
    _xlabel(xlabel),
    _ylabel(ylabel),    
    _initialized(false),
    _head_ptr(NULL),
    _head_last(NULL),
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
    
    ProcessEpicsData();
    
    ++frame;
}

void PlotWindow::ProcessEpicsData() {
    // not initialized via addPV yet
    if(_head_ptr == NULL)
        return;
    
    // get the pointer to the most recent item 
    // in the list (snapshot of current state)
    Epics::DataItem* head = *_head_ptr;
    
    // is there something new in
    // the linked list since the last 
    // call?
    if(_head_last != NULL && _head_last == head) 
        return;   
    
    // scan the linked list
    typedef vector<Epics::DataItem*> list_t;
    list_t list;
    Epics::fillList(head, list);
    
    // go thru the vector in positive time direction (ie reverse direction)
    // note that the linked list (scanned above) starts from the head (most recent item!)
    bool newData = false;
    for(list_t::reverse_iterator it=list.rbegin(); // reverse begin
        it!=list.rend(); // reverse end
        ++it) {
        
        Epics::DataItem* i = (*it);
        if(i->prev == _head_last) {
            newData = true;
        }      
        
        if(!newData)
            continue;
        
        // if new, process it!
        switch (i->type) {
        case Epics::Connected:
            //cout << "PV " << _pvname << " connected" << endl;
            break;
            
        case Epics::Disconnected:
            //cout << "PV " << _pvname << " disconnected" << endl;
            break;
            
        case Epics::NewValue: {
            vec2_t* d = (vec2_t*)i->data;
            //cout << "New Value " << d->x << " " << d->y << endl;                
            graph.AddToBlockList(*d);
            break;               
        }
        default:
            break;
        }        
    }
    // remember the last head for the next call
    _head_last = head;
    
    // do not delete the last two elements, 
    // which are still needed to build the list atomically
    for(list_t::reverse_iterator it=list.rbegin(); // reverse begin
        it<list.rend()-2; // reverse end, but not the last two!
        ++it) {
        // delete the current one properly
        Epics::DataItem* cur = *it;
        Epics::deleteDataItem(cur);   
        // and tell the next, that it's not pointing backwards to
        // us anymore
        Epics::DataItem* next = *(it+1);
        next->prev = NULL;
    }
}

int PlotWindow::Init()
{
    int ret = 0;
    _head_ptr = Epics::I().addPV(_pvname);
    if(_head_ptr==NULL) {
        ret = 1;
    }    
    // put other init code here, set ret!=0 on fail
    // but don't change it on success
    
    // return & save status
    _initialized = ret == 0;
    return ret;
}

void PlotWindow::SetYRange(const float min, const float max)
{
    graph.SetYRange( Interval(min, max ));
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
