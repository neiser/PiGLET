#include <cmath>
#include <iostream>
#include <sstream>

#include "PiGLETApp.h"
#include "arch.h"
#include "ConfigManager.h"
#include "PlotWindow.h"
#include "ImageWindow.h"

using namespace std;

void PiGLETApp::Draw(){
       
    // Start with a clear screen

    glClearColor(.1,.1,.1,0);

    glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    glLoadIdentity();

    glLineWidth(3);

    windowman.Draw();
       
    // check if there are callbacks from the telnet
    // to be executed
    ConfigManager::I().ExecutePendingCallback();
    ReportGLError();
}


void PiGLETApp::Init(){
    cout << "Starting PiGLET..." << endl;
    for (int i = 0 ; i < 3; i++){
        stringstream ss;
        ss << "MyTestRecord" << i;
        windowman.AddWindow(new PlotWindow(&windowman, ss.str()));
    }
    
    // Add one extra Image Window
    ImageWindow* w = new ImageWindow(&windowman, "Webcam");
    windowman.AddWindow(w);
}
