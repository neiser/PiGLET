#include "arch.h"
#include "PiGLETApp.h"

int main()
{
    // InitGL from arch.h
    InitGL();
    
    // create and start the app
    PiGLETApp app;
    RunGL(app);
}

