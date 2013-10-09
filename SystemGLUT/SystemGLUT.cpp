#include "SystemGLUT.h"

#include <system.h>
#include <iostream>
#include "stdlib.h"

using namespace std;


static GLApp* currentInstance;

static void drawCallback() {
    currentInstance->Draw();
    glFlush();
    glFinish();
    glutSwapBuffers();
}

void keyPressed (unsigned char key, int x, int y) {
    if( key = 27 ) {
        exit(0);
    }
}


void RunGL(GLApp &app) {
    cout << "GLUT Init" << endl;
    int iArgc = 0;
    glutInit(&iArgc, NULL);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_STENCIL );
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("PiGLPlot");
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    currentInstance = &app;
    app.Init();
    glutIdleFunc(drawCallback);
    glutKeyboardFunc(keyPressed);
    cout << "Press ESC to quit" << endl;
    glutMainLoop();
}
