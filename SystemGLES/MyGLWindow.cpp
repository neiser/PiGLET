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
#include <cmath>
#include <time.h>
using namespace std;

MyGLWindow::MyGLWindow(EGLconfig *_config) : EGLWindow(_config), graph(20), phase(0.0f), frames(0)
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

/**
 * @brief Calculate the differnce in seconds of two timespec values
 * @param start The first (earlier) time point
 * @param stop The later time point
 * @return difference in seconds
 */
float time_difference( const timespec* start, const timespec* stop ) {
    float time = stop->tv_sec - start->tv_sec + (stop->tv_nsec - start->tv_nsec) * 1E-9;
    return time;
}



void MyGLWindow::paintGL()
{
	// Start with a clear screen
    glClearColor(.1,.1,.1,0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


            glEnableClientState(GL_VERTEX_ARRAY);

            glLineWidth(3);

            glPushMatrix();

            glScalef(0.8f,0.8f,0.8f);

            glVertexPointer(2, GL_FLOAT, 0, square);

            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_STENCIL_TEST);
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            glDepthMask(GL_FALSE);
            glStencilFunc(GL_NEVER, 1, 0xFF);
            glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);  // draw 1s on test fail (always)

            // draw stencil pattern
            glStencilMask(0xFF);
            glClear(GL_STENCIL_BUFFER_BIT);  // needs mask=0xFF

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glDepthMask(GL_TRUE);
            glStencilMask(0x00);
             // draw where stencil's value is 0
             glStencilFunc(GL_EQUAL, 0, 0xFF);
             /* (nothing to draw) */
             // draw only where stencil's value is 1
             glStencilFunc(GL_EQUAL, 1, 0xFF);



             glColor4f(0,0,0,0);

             glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            glColor4f(1,1,1,1);

            glDrawArrays(GL_LINE_LOOP, 0, 4);

            graph.Draw();

            glPopMatrix();

            glDisableClientState(GL_VERTEX_ARRAY);

            glDisable(GL_STENCIL_TEST);

	glFlush();
	glFinish();
	swapBuffers();

    //cout << "xlen=" << graph.XR << "   xcenter="<<buffer.xcenter() << endl;

    phase += .02f;

    vec2_t n;
    n.x = phase;
    n.y = sin(3.14157*phase/10.0);
    graph.Add(n);
    ++frames;
    if( frames == 200 ) {
        timespec stop;
        clock_gettime(CLOCK_MONOTONIC, &stop);
        float diff = time_difference( &start_fps, &stop );

        cout << "fps: " << frames / diff << endl;
        frames = 0;
        clock_gettime(CLOCK_MONOTONIC, &start_fps);
    }


}

const vec2_t MyGLWindow::square[4] = { {1,1},{-1,1},{-1,-1},{1,-1} };
