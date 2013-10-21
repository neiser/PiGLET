#ifndef BLOCKBUFFER_H
#define BLOCKBUFFER_H

#include <vector>
#include <list>

#include "Structs.h"
#include "Interval.h"
#include "GLTools.h"

#define DATA_BLOCK_SIZE 256



class Block {
protected:
    Interval _xrange;

public:

    Block(): _xrange() {}
    virtual ~Block() {}

    const Interval& XRange() const { return _xrange; }

    virtual void Draw() const =0;

    virtual bool isFull() const { return true; }

    virtual void Add( const vec2_t& vertex ) =0;

    virtual const vec2_t& LastValue() const =0;

};

class DataBlock: public Block {
protected:
    std::vector<vec2_t> _data;
    size_t _size;

public:
    DataBlock( const unsigned int size=DATA_BLOCK_SIZE ): _size(size) { _data.reserve(size);}
    virtual ~DataBlock() {}

    virtual void Draw() const;

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
    float _backlen;
    Interval _xrange;
    Interval _yrange;

    void PopBack();


public:
    Color color;

    BlockList( const float backlen=1.0f );
    virtual ~BlockList();

    const Interval& XRange() const { return _xrange; }
    const Interval& YRange() const { return _yrange; }

    void Add( const vec2_t& vertex );

    void Draw() const;

    void SetNow( const float now );
    void SetBackLength( const float len ) { _backlen = len; 
                                            _xrange.Min() = _xrange.Max()-len; }
    const float& GetBackLenght () { return _backlen; }
    void SetYRange( const Interval& yrange ) { _yrange = yrange; }

    void NewBlock(const bool copy_last=true);

};


#endif // BLOCKBUFFER_H
