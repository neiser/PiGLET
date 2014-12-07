#ifndef GLES_H
#define GLES_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include "bcm_host.h"

void InitGL();
void RunGL();

int GetWindowWidth();
int GetWindowHeight();
void ReportGLError();

// add this missing command on GLES arch
inline void glColor4fv( const float* c ) {
    glColor4f( c[0], c[1], c[2], c[3]);
}

#endif
