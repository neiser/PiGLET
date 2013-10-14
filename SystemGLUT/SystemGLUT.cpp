#include "SystemGLUT.h"

#include <system.h>
#include <iostream>
#include "stdlib.h"

using namespace std;


static GLApp* currentInstance;
static bool fullscreen = false;

static void drawCallback() {
    currentInstance->Draw();
    glFlush();
    glFinish();
    glutSwapBuffers();
}

void toggleFullscreen() {
    if(fullscreen) {
        glutPositionWindow(0,0);
        fullscreen=false;
    } else {
        glutFullScreen();
        fullscreen = true;
    }
}

void keyPressed (unsigned char key, int x, int y) {
    switch (key) {
    case 27:
        exit(0);
        break;
    case 'f':
        toggleFullscreen();
        break;
    default:
        break;
    }
}

void InitGL(){
    cout << "GLUT Init" << endl;
    int iArgc = 0;
    glutInit(&iArgc, NULL);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_STENCIL );
    glutInitWindowSize(720, 720);

    glutInitWindowPosition(0, 0);
    glutCreateWindow("PiGLPlot");
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glutIdleFunc(drawCallback);
    glutKeyboardFunc(keyPressed);
}

void RunGL(GLApp &app) {
    currentInstance = &app;
    app.Init();
    cout << "Press ESC to quit" << endl;
    glutMainLoop();
}
