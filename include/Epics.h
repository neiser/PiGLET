#ifndef EPICS_H
#define EPICS_H

#include <map>
#include <vector>
#include <string>
#include <cadef.h>
#include "Callback.h"

using util::Callback; // Callback lives in the util namespace

class Epics  {
    
public:   
    
    typedef enum {
        Connected,
        Disconnected,
        NewValue,
        NewProperties
    } DataType;
    
       
    typedef struct DataItem {
        DataType type;
        void* data;
        struct DataItem* prev;
    } DataItem;
        
    typedef Callback<void (const DataItem* i)> EpicsCallback;
    
    void addPV(const std::string& pvname, EpicsCallback cb); // returns the tail of the datalist
    void removePV(const std::string& pvname);
    void processNewDataForPV(const std::string& pvname);
    
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
    
    static void deleteDataItem(DataItem* i);
    static void fillList(DataItem* head, std::vector<DataItem*>& list);
    
private:
    
    Epics ();
    ~Epics ();  
    void init();
    void close();
    // Singleton: Stop the compiler generating methods of copy the object
    Epics(Epics const& copy);            // Not Implemented
    Epics& operator=(Epics const& copy); // Not Implemented
    
    typedef struct PV {
        chid chid_val;
        evid evid_val;
        chid chid_ctrl;
        evid evid_ctrl; 
        DataItem* head_last; // remember the last head since processing
        EpicsCallback cb;
        DataItem*  head; // accessed by EPICS callbacks, one data stream per PV
    } PV;
    
    static PV* initPV();
    
    // Storage for channels/subscriptions
    std::map<std::string, PV*> pvs;
    
    epicsTime t0;
    
    
    static void connectionCallback( connection_handler_args args );
    static void eventCallback( event_handler_args args );
    static void exceptionCallback( exception_handler_args args );
    static void appendToList(PV* pv, DataItem* pNew);
    
    static void subscribe(const std::string &pvname, PV* pv);        
        
};


#endif
