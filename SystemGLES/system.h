#ifndef SYSTEM_H
#define SYSTEM_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include "SystemGLES.h"


inline void glColor4fv( const float* c ) {
    glColor4f( c[0], c[1], c[2], c[3]);
}

#endif // SYSTEM_H
