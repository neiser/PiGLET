#include <iostream>
#include <sstream>
#include <string.h> // for strcmp
#include <cmath>
#include "Callback.h"
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
    WindowArea( dBackColor, dWindowBorderColor),
    graph(this, 60), // DEFAULT_BACKLEN 60
    text(this, -0.98, .66, .99, .98),
    frame(0),
    _epics_connected(false),
    discon_lbl(this, -.1, -.1, .8, .9)
{
    //cout << "Plotwindow ctor" << endl;
    text.SetText(pvname);
    discon_lbl.SetColor(kPink);
    discon_lbl.SetText("Disconnected");
    
    // don't forget to call Init()

}

int PlotWindow::Init()
{
    if(_pvname.empty())
        return 1;
    
    ConfigManager::I().addCmd(Name()+"_BackLength", BIND_MEM_CB(&PlotWindow::callbackSetBackLength, this));    
    
    int ret = Window::Init();
    // the provided cb is triggered via processNewDataForPV    
    Epics::I().addPV(_pvname, BIND_MEM_CB(&PlotWindow::ProcessEpicsData, this));     
    // return & save status for dtor    
    _initialized = ret == 0;
    return ret;
}

PlotWindow::~PlotWindow() {
    if(_initialized) {
        Epics::I().removePV(_pvname);      
    }
    ConfigManager::I().removeCmd(_pvname+"_BackLength");
    //cout << "Plotwindow dtor" << endl;
} 

string PlotWindow::callbackSetBackLength(const string& arg){
    graph.SetBackLength(atoi(arg.c_str()));
    return ""; // success
}

void PlotWindow::Draw() {
    
    graph.SetNow(Epics::I().GetCurrentTime());
    Epics::I().processNewDataForPV(_pvname);   
    
   
    // Window border
    WindowArea.Draw();
    graph.Draw();
    text.Draw();

    if( !_epics_connected ) {
        discon_lbl.Draw();
    }
     
    ++frame;
}

void PlotWindow::ProcessEpicsData(const Epics::DataItem* i) {

    // if new, process it!
    switch (i->type) {
    case Epics::Connected:
        _epics_connected = true;        
        graph.enable_lastline = true;
        break;
        
    case Epics::Disconnected:
        // if we were connected before
        // start a new block
        if( _epics_connected ) {
            graph.NewBlock();
        }
        _epics_connected = false;
        graph.enable_lastline = false;
        break;
        
    case Epics::NewValue: {
        vec2_t* d = (vec2_t*)i->data;
        graph.AddToBlockList(*d);     
        break;               
    }
    case Epics::NewProperties: {
        ProcessEpicsProperties(i->attr, i->data);
        break;
    }
    }        
    
}

void PlotWindow::ProcessEpicsProperties(const string& attr, void* d) {
    
    if(attr == "HIHI") {
        graph.SetMajorAlarmsMax(*(dbr_double_t*)d);
    }
    else if(attr == "HIGH") {
        graph.SetMinorAlarmsMax(*(dbr_double_t*)d);
    }
    else if(attr == "LOW") {
        graph.SetMinorAlarmsMin(*(dbr_double_t*)d);
    }
    else if(attr == "LOLO") {
        graph.SetMajorAlarmsMin(*(dbr_double_t*)d);
    }    
    else if(attr == "SEVR") {
        graph.SetAlarm((epicsAlarmSeverity)(*(dbr_enum_t*)d));
    }
    else if(attr == "LOPR") {
        graph.SetYRangeMin(*(dbr_double_t*)d);
    }
    else if(attr == "HOPR") {
        graph.SetYRangeMax(*(dbr_double_t*)d);
    }
    else if(attr == "EGU") {
        // append the unit to the title
        stringstream title;
        string u((char*)d);
        title << _pvname;
        if(!u.empty()) {
            title << " / " << u;
            text.SetText(title.str());
        }        
    }
    else if(attr == "PREC") {
        short prec = *(dbr_short_t*)d;
        if(prec<=0)
            return;
        graph.SetPrecision(prec);
    }
    else {
        cout << "Attribute change " << attr << " ignored." << endl;
    }
}

void PlotWindow::Update() 
{ 
    graph.UpdateTicks(); 
}




std::ostream& operator<<( std::ostream& stream, const PlotWindow& win ) {
    stream << "[ " << win.XPixels() << " x " << win.YPixels() << " ]: " << win.Xlabel() << " vs. " << win.Ylabel();
    return stream;
}
