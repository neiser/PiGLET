#include "arch.h"
#include "Sound.h"
int main()
{
    // play just 2 seconds of silence,
    // fixes weird sound issue on RPi
    Sound::I().Play("silence");
    
    // InitGL from arch.h
    // it basically calls PiGLETApp::I().Init()
    // and some other GL specific init stuff...
    InitGL();
    
    // create and start the app
    // it basically calls PiGLETApp::I().Draw()
    // around GL specific stuff
    RunGL();
}

