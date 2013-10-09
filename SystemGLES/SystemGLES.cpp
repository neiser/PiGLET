#include "SystemGLES.h"

void RunGL() {
    std::cout<<"starting GL test\n";
  bcm_host_init();
    std::cout<<"done bcm init\n";
    std::cout<<"move mouse to change colour, mouse button to exit\n";
    // here I create a config with RGB bit size 5,6,5 and no alpha
    EGLconfig *config = new EGLconfig();
    config->setRGBA(0,0,0,0);
    // set the depth buffer
    config->setDepth(24);

    // enable stencil buffer
    config->setAttribute(EGL_STENCIL_SIZE,1);
    // now create a new window using the default config
    MyGLWindow win(config);

    while(1)
    {

        win.paintGL();
    }

    bcm_host_deinit();
}
