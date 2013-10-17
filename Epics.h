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
    enum CallbackMode {
        Connected,
        Disconnected,
        NewValue
    };
    
    typedef Callback<void (const CallbackMode&, const double&, const double&)> EpicsCallback;
    void addPV(const std::string& pvname, EpicsCallback cb);
    void removePV(const std::string& name);
    
    // Implement a singleton
    static Epics& I() {
        // Returns the only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static Epics instance;
        return instance;
    }
     
    // the reference timepoint for 
    // timestamps given to the
    // epics callbacks
    double GetCurrentTime();
    
    void MutexLock();
    void MutexUnlock();
    
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
        EpicsCallback   cb;
    };
        
    // Storage for values
    std::map<std::string, PV*> pvs;
    
    epicsTime t0;
    
    pthread_mutex_t _mutex;
    
    static void connectionCallback( connection_handler_args args );
    static void eventCallback( event_handler_args args );
    static void exceptionCallback( exception_handler_args args );
};


#endif
