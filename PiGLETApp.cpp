#include <cmath>
#include <iostream>
#include <sstream>

#include "PiGLETApp.h"
#include "system.h"

using namespace std;

void PiGLETApp::Draw(){
    // Start with a clear screen

    glClearColor(.1,.1,.1,0);

    glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    glLoadIdentity();

    glLineWidth(3);

    windowman.Draw();
    
    ReportGLError();
}
void PiGLETApp::Init(){
    cout << "Starting a test app!" << endl;
    for (int i = 0 ; i < 3; i++){
        stringstream ss;
        //ss << "DAQ:Livetime-vme-exptrigger " << i;
        ss << "MyTestRecord" << i;
        windowman.AddWindow( new PlotWindow(ss.str()));
    }

    //Add one extra Image Window
    ImageWindow* w = new ImageWindow("Image");
    w->SetURL("https://i.chzbgr.com/maxW500/7846712320/h4B84CC02/");
    windowman.AddWindow(w);
}
