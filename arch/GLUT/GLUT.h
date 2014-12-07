#ifndef GLUT_H
#define GLUT_H

#include <GL/glut.h>

#define DEFAULT_WINDOW_WIDTH    1024
#define DEFAULT_WINDOW_HEIGHT   1024/1.6

void InitGL();
void RunGL();

int GetWindowWidth();
int GetWindowHeight();

void ReportGLError();

#endif
