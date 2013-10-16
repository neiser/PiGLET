
#include <iostream>
#include "system.h"

#include "PiGLPlotApp.h"
#include "ConfigManager.h"
#include "Epics.h"

using namespace PiGLPlot;

int main()
{
    
    // system-specific InitGL
    InitGL();
    // ..now some system-wide GL stuff
    // enable the vertex array always, since this is always used
    // but the GL_TEXTURE_COORD_ARRAY must not be enabled globally (otherwise: segfault!)
    glEnableClientState(GL_VERTEX_ARRAY);
    //  PiGLPlotApp app;
    PiGLPlotApp app;
//    PiGLPlotApp app;
    RunGL(app);
}

