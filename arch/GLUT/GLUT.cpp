#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "GLUT.h"
#include "arch_common.h"
#include "PiGLETApp.h"

using namespace std;

static bool fullscreen = false;
static int _width =  DEFAULT_WINDOW_WIDTH;
static int _height = DEFAULT_WINDOW_HEIGHT;


void Reshape( int w, int h ) {
    _width = w;
    _height = h;
}

int GetWindowWidth() {
    return _width;
}

int GetWindowHeight() {
    return _height;
}

static void drawCallback(int val) {
    PiGLETApp::I().Draw();
    glFlush();
    glFinish();
    glutSwapBuffers();
    glutTimerFunc(20, drawCallback, 0);
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
    glutInitWindowSize(_width, _height);

    glutInitWindowPosition(0, 0);
    glutCreateWindow("PiGLET");
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glutTimerFunc(0, drawCallback, 0);
    glutKeyboardFunc(keyPressed);
//    glutReshapeFunc(Reshape);
    CommonInitGL();
}

void RunGL() {
    PiGLETApp::I().Init();
    cout << "Press ESC to quit" << endl;
    glutMainLoop();
}

void ReportGLError() {
    GLenum err = glGetError();
    if(err != GL_NO_ERROR) {
        cerr << "OpenGL Error (fix that!): " << gluErrorString(err) << endl;        
    }
    
}
