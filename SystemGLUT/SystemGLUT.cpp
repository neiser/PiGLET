#include "SystemGLUT.h"

#include <system.h>
#include <iostream>

using namespace std;


static GLApp* currentInstance;

static void drawCallback() {
    currentInstance->Draw();
    glFlush();
    glFinish();
    glutSwapBuffers();
}


void RunGL(GLApp &app) {
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
    glutIdleFunc(drawCallback);
    glutMainLoop();
}
