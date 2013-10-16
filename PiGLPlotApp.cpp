#include "PiGLPlotApp.h"
#include "system.h"

#include <cmath>
#include <iostream>
#include <sstream>

using namespace std;

void PiGLPlotApp::Draw(){
    // Start with a clear screen

    glClearColor(.1,.1,.1,0);

    glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    glLoadIdentity();

    glLineWidth(3);

    windowman.Draw();
    
    ReportGLError();
}
void PiGLPlotApp::Init(){
    cout << "Starting a test app!" << endl;
    for (int i = 0 ; i < 3; i++){
        stringstream ss;
        ss << "DAQ:Livetime-vme-exptrigger " << i;
        windowman.AddWindow( new PlotWindow(ss.str()));
    }
}
