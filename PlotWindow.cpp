#include <iostream>
#include <sstream>
#include <string.h> // for strcmp

#include "PlotWindow.h"
#include "ConfigManager.h"

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
    frame(0),
    _old_properties()
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
   
    // we use this macro only in this function 
    // to prevent typos in the field names   
    #define CHANGED(field) ((d->field) != (_old_properties.field))  
    
    // set alarm/warnings levels
    if(CHANGED(lower_alarm_limit) || CHANGED(upper_alarm_limit)) 
        graph.SetMajorAlarms(Interval(d->lower_alarm_limit, d->upper_alarm_limit));
    
    if(CHANGED(lower_warning_limit) || CHANGED(upper_warning_limit))
        graph.SetMinorAlarms(Interval(d->lower_warning_limit, d->upper_warning_limit));
        
    // set alarm state
    if(CHANGED(severity))
        graph.SetAlarm((epicsAlarmSeverity)d->severity);
    
    // display limits a.k.a yrange
    if(CHANGED(lower_disp_limit) || CHANGED(upper_disp_limit)) {
        Interval y(d->lower_disp_limit,d->upper_disp_limit);
        // if the provided interval is empty,
        // try guessing some better one
        if(y.Length()==0) {
            y = d->value==0 ? Interval(-1,1) :
                              Interval(d->value/2.0, d->value*2.0);
        }
        graph.SetYRange(y);
    }
    
    // set precision
    // only positive numbers are allowed
    if(CHANGED(precision) && d->precision>0) {
        graph.SetPrecision(d->precision);
    }
    
    // append unit to title
    // d->units is a char array, 
    // so the simple CHANGED can't be used... 
    if(strcmp(d->units,_old_properties.units) != 0) {
        // update title with unit
        stringstream title;
        string u(d->units);
        title << _pvname;
        if(!u.empty()) {
            title << " / " << u;
        }
        text.SetText(title.str());
    }    

    // save a copy of the old state
    _old_properties = *d;
    
    #undef CHANGED
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




std::ostream& operator<<( std::ostream& stream, const PlotWindow& win ) {
    stream << "[ " << win.XPixels() << " x " << win.YPixels() << " ]: " << win.Xlabel() << " vs. " << win.Ylabel();
    return stream;
}
