#include "SystemGLES.h"
#include <iostream>

static MyGLWindow* win;

void InitGL() {
    std::cout << "Starting InitGL" << std::endl;
    bcm_host_init();
    // here I create a config with RGB bit size 5,6,5 and no alpha
    EGLconfig *config = new EGLconfig();
    config->setRGBA(0,0,0,0);
    // set the depth buffer
    config->setDepth(24);

    // enable stencil buffer
    config->setAttribute(EGL_STENCIL_SIZE,1);
    // now create a new window using the default config
    win = new MyGLWindow(config);
}

void RunGL(GLApp& app) {
    app.Init();
    win->app = &app;
    while(1) {
        win->paintGL();
    }
    bcm_host_deinit();
}

void GetWindowWidth() {
    return win->getWidth();
}

void GetWindowHeight() {
    return win->getHeight();
}
