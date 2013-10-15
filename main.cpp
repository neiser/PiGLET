
#include <iostream>
#include "system.h"

#include "PiGLPlotApp.h"
#include "ConfigManager.h"

int main()
{


    // start the config manager
    ConfigManager::I().go();



    // system-specific InitGL
    InitGL();
    // ..now some system-wide GL stuff
    // enable the vertex array always, since this is always used
    // but the GL_TEXTURE_COORD_ARRAY must not be enabled globally (otherwise: segfault!)
    glEnableClientState(GL_VERTEX_ARRAY);
    //  PiGLPlotApp app;
    testApp app;
//    PiGLPlotApp app;
    RunGL(app);
}

