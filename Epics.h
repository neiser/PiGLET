//******************************************************************************
//! @file    Epics.h
//!
//! @author  F. Feldbauer, A. Neiser
//!
//! @brief   Class for EPICS interfacing
//!
//! @version 1.0.0
//******************************************************************************

#ifndef EPICS_H
#define EPICS_H

//_____ I N C L U D E S ________________________________________________________
// System headers
#include <map>
#include <vector>

//_____ D E F I N I T I O N S __________________________________________________
namespace PiGLPlot {

// define max length for EPICS PV names
const size_t MAX_PV_NAME_LEN = 40;

// Forward declaration
struct PV;
typedef struct PV PV;

class Epics  {
public:   
    
    void init();
    void close();
        
    static Epics& I() {
        // Returns the only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static Epics instance;
        return instance;
    }
    
private:
    enum channelType { Tset, PumpSet, Switch, Mean };
    
    Epics ();
    // Stop the compiler generating methods of copy the object
    Epics(Epics const& copy);            // Not Implemented
    Epics& operator=(Epics const& copy); // Not Implemented
    ~Epics ();
    
    void subscribe( std::string&, PV* );
    void createChannel( std::string&, PV*, channelType );
    
    // Storage for values
    std::vector<PV*> pvs;
    
    bool isInitialized_;
};

}

#endif

//******************************************************************************
//! EOF
//******************************************************************************
