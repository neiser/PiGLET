#ifndef EPICS_H
#define EPICS_H

#include <map>
#include <vector>
#include <string>
#include <cadef.h>
#include "Callback.h"
#include "StopWatch.h"

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
        std::string attr; // if type==NewProperties, this contains what property is in data 
        struct DataItem* prev;
    } DataItem;
        
    typedef Callback<void (const DataItem* i)> EpicsCallback;
    
    void addPV(const std::string& pvname, EpicsCallback cb, bool autoCall = false); // returns the tail of the datalist
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
    
    
    
private:
    
    Epics ();
    ~Epics ();  
    void init();
    void close();
    // Singleton: Stop the compiler generating methods of copy the object
    Epics(Epics const& copy);            // Not Implemented
    Epics& operator=(Epics const& copy); // Not Implemented
    
    typedef struct PV_channel_t {
        chid _chid;
        evid _evid;
        std::string _attr;
    } PV_channel_t;
    
    typedef struct PV {
        std::vector< PV_channel_t > channels;
        EpicsCallback cb; // gets called by processNewDataForPV() if there is new data
        bool auto_call;  // if an EPICS callback was received, the events will be processed immediately
        DataItem* head_last; // remember the last head since processing        
        DataItem* head; // accessed by EPICS callbacks, one data stream per PV
    } PV;
    
    static PV* initPV();
    
    // Storage for channels/subscriptions
    std::map<std::string, PV*> pvs;
    
    epicsTime t0;
    StopWatch _watch;
    
    
    static void processNewDataForPV(PV* pv);
    
    static void connectionCallback( connection_handler_args args );
    static void eventCallback( event_handler_args args );
    static void exceptionCallback( exception_handler_args args );
    static void appendToList(PV* pv, DataItem* pNew);
    
    static void subscribe(const std::string &pvname, PV* pv);   
    static void subscribe_channel(const std::string &pvname, PV* pv, 
                                  const std::string &attr, chtype type ); 

    static void deleteDataItem(DataItem* i);
    static void fillList(DataItem* head, std::vector<DataItem*>& list);   
        
};


#endif
