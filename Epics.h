#ifndef EPICS_H
#define EPICS_H

#include <map>
#include <vector>

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
    // Singleton: Stop the compiler generating methods of copy the object
    Epics(Epics const& copy);            // Not Implemented
    Epics& operator=(Epics const& copy); // Not Implemented
    
    void subscribe(const std::string&, PV* );
    
    // Storage for values
    std::vector<PV*> pvs;
    
    bool isInitialized_;
};

}

#endif
