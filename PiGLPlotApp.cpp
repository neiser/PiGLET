#include "PiGLPlotApp.h"

PiGLPlotApp::PiGLPlotApp() {}

void PiGLPlotApp::Init() {

}

void PiGLPlotApp::Draw() {

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
}

