
#include <iostream>

#ifdef BUILD_FOR_PI
#include "SystemGLES/SystemGLES.h"
#else
#include "SystemGLUT/SystemGLUT.h"
#endif


int main()
{
    RunGL();
}

