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
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(250, 250);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("PiGLPlot");
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    cout << "Test" << endl;
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
    currentInstance = &app;
    glutIdleFunc(drawCallback);
    glutMainLoop();
}
