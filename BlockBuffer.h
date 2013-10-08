#ifndef BLOCKBUFFER_H
#define BLOCKBUFFER_H

#include <vector>
#include <list>

#include "Structs.h"
#include "Interval.h"

namespace PiGLPlot {

class Block {
protected:
    Interval _xrange;

public:

    Block(): _xrange() {}
    virtual ~Block() {}

    const Interval& XRange() const { return _xrange; }

    virtual void Draw() =0;

    virtual bool isFull() const { return true; }

    virtual void Add( const vec2_t& vertex ) =0;

    virtual const vec2_t& LastValue() const =0;

};

class DataBlock: public Block {
protected:
    std::vector<vec2_t> _data;
    size_t _size;

public:
    DataBlock( const unsigned int size=100 ): _size(size) { _data.reserve(size);}
    virtual ~DataBlock() {}

    virtual void Draw();

    virtual void Add( const vec2_t& vertex );

    inline size_t Size() const { return _data.size(); }

    virtual vec2_t& at( const unsigned int n ) { return _data.at(n); }
    virtual const vec2_t& at( const unsigned int n ) const { return _data.at(n); }

    virtual bool isFull() const { return _data.size() >= _size; }

     virtual const vec2_t& LastValue() const { return _data.back(); }

};


class BlockList {
protected:
    typedef std::list<Block*> blist;
    blist _blocks;
    Interval _xrange;

    void PopBack();
    void NewBlock();

public:
    BlockList( const Interval& xrange );

    void Add( const vec2_t& vertex );

    void Draw();

};
}

#endif // BLOCKBUFFER_H
