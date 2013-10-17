#include "system.h"
#include "PiGLETApp.h"

int main()
{
    // system-specific InitGL
    InitGL();
    // ..now some system-wide GL stuff
    // enable the vertex array always, since this is always used
    // but the GL_TEXTURE_COORD_ARRAY must not be enabled globally (otherwise: segfault!)
    glEnableClientState(GL_VERTEX_ARRAY);
    glMatrixMode(GL_MODELVIEW); // use for the following the much bigger Modelview stack
    
    // create and start the app
    PiGLETApp app;
    RunGL(app);
}

