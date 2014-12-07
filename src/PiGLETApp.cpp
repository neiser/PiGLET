#include <iostream>
#include <sstream>

#include "PiGLETApp.h"
#include "arch.h"
#include "ConfigManager.h"
//#include "PlotWindow.h"
//#include "ImageWindow.h"
#include <sys/prctl.h>

using namespace std;

#define AVG_FRAMES 200

void PiGLETApp::Draw(){
       
    // Start with a clear screen
    glClearColor(.1,.1,.1,0);
    glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glLoadIdentity();
    glLineWidth(3);

    // draw the stuff, 
    // but don't let the config manager interfere
    ConfigManager::I().MutexLock();    
    windowman.Draw();
    ConfigManager::I().MutexUnlock();
       
    // check if there are callbacks from the telnet
    // to be executed
    ConfigManager::I().ExecutePendingCallback();
    ReportGLError();
    
    frames++;
    if(frames % AVG_FRAMES == 0) {
        frames_timer.Stop();        
        
        fps = AVG_FRAMES/(frames_timer.TimeElapsed()-timeElapsed);
        cout << " FPS: " << fps << endl;
        
        timeElapsed = frames_timer.TimeElapsed();
    }
}


double PiGLETApp::GetRoughTime()
{
    return timeElapsed +
            (frames % AVG_FRAMES)/fps;
}

void PiGLETApp::Init(){
    prctl(PR_SET_NAME, "PiGLET", 0l, 0l, 0l);
    cout << "Starting PiGLET..." << endl;
    frames_timer.Start();
    frames = 0;
    timeElapsed = 0.0;
    fps = 25.0; // some guess for initial frames
        
//    for (int i = 0 ; i < 1; i++){
//        stringstream ss;
//        ss << "MyTestRecord" << i;
//        windowman.AddWindow(new PlotWindow(&windowman, ss.str()));
//    }
 //   windowman.AddWindow(new PlotWindow(&windowman, "BEAM:IonChamber"));
//    // Add one extra Image Window
//    ImageWindow* w = new ImageWindow(&windowman, "Webcam");
//    windowman.AddWindow(w);
}
