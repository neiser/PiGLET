#include "BlockBuffer.h"


#include <iostream>
#include <iomanip>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <cmath>

using namespace std;

void PiGLPlot::DataBlock::Draw()
{
    glVertexPointer(2, GL_FLOAT, 0, _data.data());
    glDrawArrays(GL_LINE_STRIP, 0, _data.size());
}

void PiGLPlot::DataBlock::Add(const vec2_t &vertex)
{
    if( _data.empty() )
        _xrange.Min() = vertex.x;

    _xrange.Max() = vertex.x;

    _data.push_back(vertex);

}


void PiGLPlot::BlockList::PopBack()
{
    Block* last = _blocks.back();
    delete last;
    _blocks.pop_back();
    cout << "delete!" << endl;
}

void PiGLPlot::BlockList::NewBlock()
{

    Block* b = new DataBlock();

    if( !_blocks.empty() ) {
        Block* l = _blocks.front();
        b->Add( l->LastValue() );
    }

    _blocks.push_front(b);

}

PiGLPlot::BlockList::BlockList(const float backlen): _backlen(backlen)
{
}

void PiGLPlot::BlockList::Add(const vec2_t &vertex)
{
    if( _blocks.empty() )
        NewBlock();

    Block* h = _blocks.front();

    if( h->isFull() ) {
        NewBlock();
        h = _blocks.front();
    }

    h->Add( vertex );

    _xrange = Interval(vertex.x - _backlen, vertex.x );

    Block* last = _blocks.back();

    if( last->XRange().Disjoint(_xrange) ) {
        PopBack();
    }

}

void PiGLPlot::BlockList::Draw()
{

    glPushMatrix();

    glScalef( 2.0 / _xrange.Length(), 1 ,1);
    glTranslatef(-_xrange.Center(), 0, 0);

    DoTicks();

    blist::iterator i;

    glColor4f(1,1,1,0);
    for( i= _blocks.begin(); i != _blocks.end(); ++i )
        (*i)->Draw();

    glPopMatrix();

}

void PiGLPlot::BlockList::DoTicks()
{
    glColor4f(.4,.4,.4,0);
    float tickd = 5;
    float tick = floor((_xrange.Max() / tickd))*tickd;



    int nt = _xrange.Length() / tickd;
    vector<vec2_t> ticks;
    ticks.reserve( nt * 2 );
  //  cout << "xmax: " << _xrange.Max() << " 1:" << tick << " " << nt << endl;

    for( int i=0;i<nt;++i){
        vec2_t t;
        t.x = tick - tickd * i;
        t.y = -1;
        ticks.push_back(t);
        t.y = 1;
        ticks.push_back(t);
    }

    glVertexPointer(2, GL_FLOAT, 0, ticks.data());
    glDrawArrays(GL_LINES, 0, ticks.size());

}
