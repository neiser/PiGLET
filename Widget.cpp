#include "Widget.h"

#include "system.h"
#include "Window.h"

#include <cmath>
#include <iostream>

using namespace std;

float SimpleGraph::dXticks( const float& xlen, const int& nt ){
    float dx = 1.;
    if ( xlen / dx > nt){
        while ( true ){
            dx*=2;
            if ( xlen / dx <= nt ) return dx;
            dx*=5./2;
            if ( xlen / dx <= nt ) return dx;
            dx*= 20./ 5.;
            if ( xlen / dx <= nt ) return dx;
        }
    }
    while ( true ){
      dx*=0.5;
      if ( xlen / dx > nt ) return dx;
      dx*=2./5;
      if ( xlen / dx > nt ) return dx;
      dx*= 5. / 20;
      if ( xlen / dx > nt ) return dx;
    }
    return dx;
}

void SimpleGraph::DrawTicks()
{
    glPushMatrix();

    const float xlen = _blocklist.GetBackLenght();

    glScalef( 2.0 / xlen, 1 ,1);
    glColor4f(.6,.6,.6,0);

    const int nt = _owner->XPixels() / 150;
    vector<vec2_t> ticks;
    ticks.reserve( nt * 2 );

    float dx = dXticks(xlen,nt);

    for( int i=0 ; i< xlen / dx ; ++i){
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
//MakeXLabels()
}

void SimpleGraph::Draw()
{
    _blocklist.Draw();
    DrawTicks();
}


float Widget::GetWindowAspect() const
{
    return _owner->XPixels() / _owner->YPixels();
}
