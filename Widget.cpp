#include "Widget.h"

#include "system.h"
#include "Window.h"

#include <cmath>


using namespace std;

void SimpleGraph::DrawTicks()
{
    glPushMatrix();

    float xlen = _blocklist.XRange().Length();

    glScalef( 2.0 / xlen, 1 ,1);
    glColor4f(.6,.6,.6,0);

    int nt = 5 * _owner->XScale();
    vector<vec2_t> ticks;
    ticks.reserve( nt * 2 );

    float dx = xlen / ( nt + 1 ) ;

    for( int i=0 ; i<nt ; ++i){
        vec2_t t;
        t.x = xlen / 2 - ( i + 1 ) * dx;
        t.y = -1;
        ticks.push_back(t);
        t.y = 1;
        ticks.push_back(t);
    }

    glVertexPointer(2, GL_FLOAT, 0, ticks.data());
    glDrawArrays(GL_LINES, 0, ticks.size());
    glPopMatrix();
}

void SimpleGraph::Draw()
{
    _blocklist.Draw();
    DrawTicks();
}
