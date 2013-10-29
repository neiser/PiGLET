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
    if( _data.empty() ) {

        _xrange.Min() = vertex.x;

        _yrange.Min() = vertex.y;
        _yrange.Max() = vertex.y;

    } else {
        _yrange.Extend(vertex.y);
    }

    _xrange.Max() = vertex.x;

    _data.push_back(vertex);

}


void BlockList::BuildYRange()
{
    _yrange = Interval(0,0);
    blist::const_iterator i;
    for( i= _blocks.begin(); i != _blocks.end(); ++i )
        _yrange.Extend( (*i)->YRange() );

}

void BlockList::PopBack()
{
    if( !_blocks.empty() ) {
        Block* last = _blocks.back();
        delete last;
        _blocks.pop_back();
        BuildYRange();
    }
}

void BlockList::NewBlock( const bool copy_last )
{

    Block* b = new DataBlock();

    if( !_blocks.empty() && copy_last ) {
        Block* l = _blocks.front();
        b->Add( l->LastValue() );
    }

    _blocks.push_front(b);

}

BlockList::BlockList( const float backlen ):
    _backlen(backlen),
    _xrange(-_backlen, 0.0f),
    _yrange(.0f,.0f),
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
    if(last != h && last->XRange().Disjoint(_xrange) ) {
        PopBack();
    }

    _yrange.Extend( vertex.y );

}

void BlockList::Draw() const
{



        color.Activate();

        blist::const_iterator i;
        for( i= _blocks.begin(); i != _blocks.end(); ++i )
            (*i)->Draw();



}

void BlockList::SetNow(const float now)
{
    _xrange.Max() = now;
    _xrange.Min() = now - _backlen;
}



