#include "BlockBuffer.h"

#include "system.h"
#include <cmath>
#include <iostream>
#include <iomanip>

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

PiGLPlot::BlockList::BlockList(const float backlen): _backlen(backlen), color(dPlotColor)

PiGLPlot::BlockList::~BlockList()
{
    blist::iterator i;
    for( i= _blocks.begin(); i != _blocks.end(); ++i )
        delete *i;
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

  //  _xrange = Interval(vertex.x - _backlen, vertex.x );

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

    blist::iterator i;

    color.Activate();

    for( i= _blocks.begin(); i != _blocks.end(); ++i )
        (*i)->Draw();

    glPopMatrix();

}

void PiGLPlot::BlockList::SetNow(const float now)
{
    _xrange.Max() = now;
    _xrange.Min() = now - _backlen;
}



