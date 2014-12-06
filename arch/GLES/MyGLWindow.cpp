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
#include "MyGLWindow.h"

#include <iostream>
#include <time.h>

using namespace std;

MyGLWindow::MyGLWindow(EGLconfig *_config) : EGLWindow(_config), frames(0)
{
	std::cout<<"My GL Window Ctor\n";
	makeSurface();
    clock_gettime(CLOCK_MONOTONIC, &start_fps);
}

MyGLWindow::~MyGLWindow()
{

}

void MyGLWindow::initializeGL()
{
    cout << "init GL" << endl;
}

void MyGLWindow::paintGL()
{
    app->Draw();

	glFlush();
	glFinish();
	swapBuffers();

}

const vec2_t MyGLWindow::square[4] = { {1,1},{-1,1},{-1,-1},{1,-1} };
