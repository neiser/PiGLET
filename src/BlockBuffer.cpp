#include "BlockBuffer.h"

#include "arch.h"
#include <cmath>
#include <iostream>
#include <iomanip>

using namespace std;

void DataBlock::Draw() const
{
    glVertexPointer(2, GL_FLOAT, 0, _data.data());
    glDrawArrays(GL_LINE_STRIP, 0, _data.size());
}

void DataBlock::Add(const vec2_t &vertex)
{
    if( _data.empty() )
        _xrange.Min() = vertex.x;

    _xrange.Max() = vertex.x;

    _data.push_back(vertex);

}


void BlockList::PopBack()
{
    if( !_blocks.empty() ) {
        Block* last = _blocks.back();
        delete last;
        _blocks.pop_back();
    }
}

void BlockList::NewBlock()
{

    Block* b = new DataBlock();

    if( !_blocks.empty() ) {
        Block* l = _blocks.front();
        b->Add( l->LastValue() );
    }

    _blocks.push_front(b);

}

BlockList::BlockList( const float backlen ):
    _backlen(backlen),
    _xrange(-_backlen, 0.0f),
    _yrange(-1.0f, 1.0f),
    color(dPlotColor)
{
}

BlockList::~BlockList()
{
    blist::iterator i;
    for( i= _blocks.begin(); i != _blocks.end(); ++i )
        delete *i;
}

void BlockList::Add(const vec2_t &vertex)
{
    if( _blocks.empty() )
        NewBlock();

    Block* h = _blocks.front();

    if( h->isFull() ) {
        NewBlock();
        h = _blocks.front();
    }

    h->Add( vertex );

    Block* last = _blocks.back();

    //see if we can disgard the last block
    if( last->XRange().Disjoint(_xrange) ) {
        PopBack();
    }

}

void BlockList::Draw() const
{

    glPushMatrix();

        glScalef( 2.0f / _xrange.Length(), 2.0f / _yrange.Length(), 1.0f );

        glTranslatef(-_xrange.Center(), -_yrange.Center(), 0.0f );

        color.Activate();

        blist::const_iterator i;
        for( i= _blocks.begin(); i != _blocks.end(); ++i )
            (*i)->Draw();

    glPopMatrix();

}

void BlockList::SetNow(const float now)
{
    _xrange.Max() = now;
    _xrange.Min() = now - _backlen;
}



