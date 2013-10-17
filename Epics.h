#ifndef EPICS_H
#define EPICS_H

#include <map>
#include <vector>
#include <string>
#include <cadef.h>




// define max length for EPICS PV names
const size_t MAX_PV_NAME_LEN = 40;

class Epics  {
    
public:   
    
    typedef enum {
        Connected,
        Disconnected,
        NewValue
    } DataType;
    
       
    typedef struct DataList {
        DataType type;
        void* data;
        struct DataList* prev;
    } DataList;
        
    DataList** addPV(const std::string& pvname); // returns the tail of the datalist
    void removePV(const std::string& pvname);
    
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
    
    static void deleteDataListItem(DataList* i);
    
private:
    
    Epics ();
    ~Epics ();  
    void init();
    void close();
    // Singleton: Stop the compiler generating methods of copy the object
    Epics(Epics const& copy);            // Not Implemented
    Epics& operator=(Epics const& copy); // Not Implemented
    
    typedef struct PV {
        chid       mychid;
        evid       myevid;
        DataList*  head; // accessed by EPICS callbacks
    } PV;
    
    
    // Storage for channels/subscriptions
    std::map<std::string, PV*> pvs;
    
    epicsTime t0;
    
    
    static void connectionCallback( connection_handler_args args );
    static void eventCallback_double( event_handler_args args );
    static void exceptionCallback( exception_handler_args args );
    static void appendToList(PV* pv, DataList* pNew);
};


#endif
