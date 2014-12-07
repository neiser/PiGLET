#include "WindowManager.h"
#include "StopWatch.h"

class PiGLETApp {

    
public:

    // Implement a singleton
    static PiGLETApp& I() {
        // Returns the only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static PiGLETApp instance;
        return instance;
    }
    
    

    void Init();
    void Draw();
    
private:
    PiGLETApp():windowman(){}
    ~PiGLETApp() {}
    
    unsigned int frames;
    double fps; // frames per second 
    StopWatch frames_timer;
    
    // Singleton: Stop the compiler generating methods of copy the object
    PiGLETApp(PiGLETApp const& copy);            // Not Implemented
    PiGLETApp& operator=(PiGLETApp const& copy); // Not Implemented
    
    WindowManager windowman;


};
