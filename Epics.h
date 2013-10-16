#ifndef EPICS_H
#define EPICS_H

#include <map>
#include <vector>
#include <string>
#include <cadef.h>

#include "Callback.h"
using util::Callback; // Callback lives in the util namespace


// define max length for EPICS PV names
const size_t MAX_PV_NAME_LEN = 40;

class Epics  {
public:   
    enum EpicsCallbackMode {
        Connected,
        Disconnected,
        NewValue
    };

    typedef Callback<void (EpicsCallbackMode, double x, double y)> EpicsCallback;
    void addPV(const std::string& name, EpicsCallback cb);
    void removePV(const std::string& name);
    
    // Implement a singleton
    static Epics& I() {
        // Returns the only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static Epics instance;
        return instance;
    }
    
private:
    
    Epics ();
    ~Epics ();  
    void init();
    void close();
    // Singleton: Stop the compiler generating methods of copy the object
    Epics(Epics const& copy);            // Not Implemented
    Epics& operator=(Epics const& copy); // Not Implemented
    
    struct PV {
      chid            mychid;
      evid            myevid;
      dbr_time_double dbrval;
      bool            connected;
    };
    
    void subscribe(const std::string&, PV* );
    
    // Storage for values
    std::vector<PV*> pvs;
    
    static void connectionCallback( connection_handler_args args );
    static void eventCallback( event_handler_args args );
    static void exceptionCallback( exception_handler_args args );
};


#endif
