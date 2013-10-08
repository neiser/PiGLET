/*
  Copyright (C) 2012 Jon Macey

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received m_a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>
#include "MyGLWindow.h"
#include "bcm_host.h"
#include <unistd.h>


int main()
{
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
        //usleep(200);
	}

bcm_host_deinit();
}

