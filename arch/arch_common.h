#ifndef ARCH_COMMON_H
#define ARCH_COMMON_H

// this function is called for all archs,
// as a last statement in their own InitGL
void CommonInitGL() {
    // ..now some system-wide GL stuff
    // enable the vertex array always, since this is always used
    // but the GL_TEXTURE_COORD_ARRAY must not be enabled globally (otherwise: segfault!)
    glEnableClientState(GL_VERTEX_ARRAY);
    glMatrixMode(GL_MODELVIEW); // use for the following the much bigger Modelview stack
}

#endif // ARCH_COMMON_H
