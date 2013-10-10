#include "Window.h"
#include <sstream>
#include <cmath>

void PlotWindow::Draw(){
    rect.Draw( GL_LINE_LOOP );

    glPushMatrix();

    glScalef(0.8f,0.8f,0.8f);

    rect.Draw(GL_LINE_LOOP);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
    glStencilFunc(GL_NEVER, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);  // draw 1s on test fail (always)

    // draw stencil pattern
    glStencilMask(0xFF);
    glClear(GL_STENCIL_BUFFER_BIT);  // needs mask=0xFF
    rect.Draw(GL_TRIANGLE_FAN);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glStencilMask(0x00);
     // draw where stencil's value is 0
     glStencilFunc(GL_EQUAL, 0, 0xFF);
     /* (nothing to draw) */
     // draw only where stencil's value is 1
     glStencilFunc(GL_EQUAL, 1, 0xFF);
     glColor4f(0,0,0,0);
     rect.Draw(GL_TRIANGLE_FAN);
      glColor4f(1,1,1,1);
      graph.Draw();
      glPopMatrix();

      glDisable(GL_STENCIL_TEST);

      text.Draw( GL_LINE_LOOP );

      num.Draw(frame);

      if( frame % 20 ) {
          std::stringstream s;
          s << "Frame " << frame;
          //rect.SetText(s.str());
      }

      vec2_t n;
      n.x = frame/100.0;
      n.y = sin(3.14157*frame/1000.0);
      graph.Add(n);

      ++frame;
}

std::ostream& operator<<( std::ostream& stream, const Window& win ) {
    stream << "[ " << win.Width() << " x " << win.Height() << " ]";
    return stream;
}


std::ostream& operator<<( std::ostream& stream, const PlotWindow& win ) {
    stream << "[ " << win.Width() << " x " << win.Height() << " ]: " << win.Xlabel() << " vs. " << win.Ylabel();
    return stream;
}
