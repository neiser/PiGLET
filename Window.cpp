#include "Window.h"
#include <sstream>
#include <cmath>
#include "Widget.h"
#include "GLTools.h"
#include <iostream>
#include <iomanip>
#include "TextRenderer.h"
#include "ConfigManager.h"
#include "WindowManager.h"

using namespace std;

PlotWindow::PlotWindow( 
        WindowManager* owner, 
        const std::string& pvname,
        const std::string& xlabel,
        const std::string& ylabel,
        const float xscale,
        const float yscale ) :
    Window(owner, pvname,xscale,yscale),
    _pvname(pvname),
    _xlabel(xlabel),
    _ylabel(ylabel),    
    _initialized(false),
    _head_ptr(NULL),
    _head_last(NULL),
    WindowArea( dBackColor, dWindowBorderColor),
    graph(this, 10),
    text(this, -.95, .82, .95, .98),
    frame(0)
{
    cout << "Plotwindow ctor" << endl;
    text.SetText(pvname);
    ConfigManager::I().addCmd(pvname+"_BackLength", BIND_MEM_CB(&PlotWindow::callback_SetBackLength, this));
    
    // don't forget to call Init()
    // which also checks if the pvname is actually valid
}

PlotWindow::~PlotWindow() {
    if(_initialized) {
        Epics::I().removePV(_pvname);      
    }
    ConfigManager::I().removeCmd(_pvname+"_BackLength");
    cout << "Plotwindow dtor" << endl;
} 

int PlotWindow::callback_SetBackLength(const string& arg){
    graph.SetBackLength(atoi(arg.c_str()));
    return 0;
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
    
    // the very first call, _head_last is NULL, thus everything is new!
    bool newData = _head_last == NULL;
    
    // go thru the vector in positive time direction (ie reverse direction)
    // note that the linked list (scanned above) starts from the head (most recent item!)    
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
        case Epics::NewProperties: {
            ProcessEpicsProperties((dbr_ctrl_double*)i->data);
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

void PlotWindow::ProcessEpicsProperties(dbr_ctrl_double* d) {
    // set alarm levels
    graph.SetMajorAlarms(Interval(d->lower_alarm_limit, d->upper_alarm_limit));
    graph.SetMinorAlarms(Interval(d->lower_warning_limit, d->upper_warning_limit));
        
    // set alarm state
    graph.SetAlarm((epicsAlarmSeverity)d->severity);
    
    Interval y(d->lower_disp_limit,d->upper_disp_limit);
    // if the provided interval is empty,
    // try guessing some better one
    if(y.Length()==0) {
        y = d->value==0 ? Interval(-1,1) :
                          Interval(d->value/2.0, d->value*2.0);
    }
    graph.SetYRange(y);

    // update title with unit
    stringstream title;
    string u(d->units);
    title << _pvname;
    if(!u.empty()) {
        title << " / " << u;
    }
    text.SetText(title.str());
}

    
int PlotWindow::Init()
{
    int ret = 0;
    _head_ptr = Epics::I().addPV(_pvname);
    if(_head_ptr==NULL) {
        ret = 1;
    }    
    // return & save status for dtor
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


ImageWindow::ImageWindow( WindowManager* owner, const std::string& title, const float xscale, const float yscale ):
    Window(owner, title, xscale, yscale),
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


int Window::callback_remove_window(const string &arg)
{
    return _owner->RemoveWindow(_name);    
}

Window::Window(WindowManager *owner, const string &name, const float xscale, const float yscale) : 
    _owner(owner),
    _name(name), 
    _x_pixels(xscale), 
    _y_pixels(yscale) {
    ConfigManager::I().addCmd(name+"_Remove",BIND_MEM_CB(&Window::callback_remove_window,this));
}

Window::~Window()
{
    ConfigManager::I().removeCmd(_name+"_Remove");
}
