#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <vector>
#include "Structs.h"
#include "Interval.h"

namespace PiGLPlot {

class RingBuffer {
private:
    std::vector<vec2_t> buffer;

    unsigned int head;
    unsigned int tail;

    inline unsigned int add( const unsigned int& x, const int& v ) const {
        return ((x+v) % buffer.size());
    }

public:

    RingBuffer( const unsigned int size );

    void Add( const vec2_t& v );

    vec2_t* ArrayPtr() { return buffer.data(); }
    size_t  ArrayLength() const { return buffer.size(); }

    inline size_t Size() const { return buffer.size()-1; }

    unsigned int NElements() const;

    void Debug() const;

    bool GetRegion1(Interval &i) const;
    bool GetRegion2(Interval &i) const;

};

}

#endif // RINGBUFFER_H
