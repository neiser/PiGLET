#include "BlockBuffer.h"


#include <iostream>
#include <iomanip>

using namespace std;

void PiGLPlot::DataBlock::Draw()
{
    cout << "[ ";
    for( int i=0; i<_data.size(); ++i )
        cout << setw(3) << _data[i].x << " ";
    cout << "] Range: " << _xrange << endl;

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

    Block* b = new DataBlock(5);

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
    blist::iterator i;

    cout << "====================== " << _blocks.size() << endl;

    for( i= _blocks.begin(); i != _blocks.end(); ++i )
        (*i)->Draw();

    cout << endl;
}
