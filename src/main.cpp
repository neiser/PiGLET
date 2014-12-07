#include "arch.h"
int main()
{
    // InitGL from arch.h
    // it basically calls PiGLETApp::I().Init()
    // and some other GL specific init stuff...
    InitGL();
    
    // create and start the app
    // it basically calls PiGLETApp::I().Draw()
    // around GL specific stuff
    RunGL();
}

