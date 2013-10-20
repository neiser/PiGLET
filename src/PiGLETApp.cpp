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
    
    ReportGLError();
    
    
    // check if there are callbacks from the telnet
    // to be executed
    ConfigManager::I().ExecutePendingCallback();
}


void PiGLETApp::Init(){
    cout << "Starting a test app!" << endl;
    for (int i = 0 ; i < 3; i++){
        stringstream ss;
        ss << "MyTestRecord" << i;
        windowman.AddWindow(new PlotWindow(&windowman, ss.str()));
    }

    //windowman.RemoveWindow(0);
    //windowman.RemoveWindow(0);
    
//    //Add one extra Image Window
    ImageWindow* w = new ImageWindow(&windowman, "Webcam");
    //w->SetURL("http://auge.physik.uni-mainz.de/record/current.jpg");
    windowman.AddWindow(w);
}
