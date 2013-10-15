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


void SimpleGraph::UpdateTicks(){
    const float xlen = _blocklist.GetBackLenght();
    //calulate rough estimate how many ticks:
    const int nt = _owner->XPixels() / 150;
    _xticks.clear();
    _xticks.reserve( nt * 2);

    //get optimized distance close to calculated:
    float dx = dXticks(xlen,nt);

    //do ticks
    for( int i=0; i < xlen ; ++i ){
        vec2_t t;
        t.x = xlen / 2 - ( i + 1 ) * dx;
        t.y = -1;
        _xticks.push_back(t);
        t.y = 1;
        _xticks.push_back(t);
    }
}

void SimpleGraph::DrawTicks()
{
    glPushMatrix();

    glScalef( 2.0 / _blocklist.GetBackLenght(), 1 ,1);
    glColor4f(.6,.6,.6,0);

    glVertexPointer(2, GL_FLOAT, 0, _xticks.data());
    glDrawArrays(GL_LINES, 0, _xticks.size());

    glPopMatrix();
}

void SimpleGraph::Draw()
{
    DrawTicks();
    _blocklist.Draw();
}


float Widget::GetWindowAspect() const
{
    return _owner->XPixels() / _owner->YPixels();
}
