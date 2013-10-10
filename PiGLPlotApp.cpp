#include "PiGLPlotApp.h"
#include "system.h"

#include <cmath>
#include <iostream>
#include <sstream>

using namespace std;

PiGLPlotApp::PiGLPlotApp(): graph(10.0), phase(0.0), rect(-.95,0.82,.95,.98), frame(0) {}

void PiGLPlotApp::Init() {

   rect.SetText("Hallo Welt");

   cout << "App init done" << endl;


}

void PiGLPlotApp::Draw() {

    // Start with a clear screen
    glClearColor(.1,.1,.1,0);

    glClear( GL_COLOR_BUFFER_BIT );


            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);

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

            glDisable(GL_STENCIL_TEST);

            rect.Draw( GL_LINE_LOOP );

            glDisableClientState(GL_VERTEX_ARRAY);



            phase += .02f;

            if( frame % 20 ) {
                stringstream s;
                s << "Frame " << frame;
                rect.SetText(s.str());
            }

            vec2_t n;
            n.x = phase;
            n.y = sin(3.14157*phase/10.0);
            graph.Add(n);

            ++frame;


}

const vec2_t PiGLPlotApp::square[4] = { {1,1},{-1,1},{-1,-1},{1,-1} };


void testApp::Draw(){
    pwin.Draw();

}
void testApp::Init(){
    cout << "Starting a test app!" << endl;
}
