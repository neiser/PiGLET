#include "RingBuffer.h"

#include <iostream>
#include <iomanip>

using namespace std;


PiGLPlot::RingBuffer::RingBuffer(const unsigned int size): buffer(size+1), head(1), tail(1)
{
}

void PiGLPlot::RingBuffer::Add(const vec2_t &v)
{
    buffer[head] = v;

    head = add(head, 1);

    if( head == tail )
        tail = add(tail,1);
}

unsigned int PiGLPlot::RingBuffer::NElements() const
{
    if( head >= tail) {
        return head - tail;
    } else {
        return (buffer.size() - tail) + head;
    }
}

void PiGLPlot::RingBuffer::Debug() const
{
    cout << "head: " << head << "  tail: " <<tail << "  [";
    for( int i =0;i<buffer.size(); ++i)
        cout << setw(3) << buffer[i].x << " ";
    cout << "]";

    Interval i;

    cout << " Region1 = ";
    if( GetRegion1(i) ) {
        cout << i;
    } else
        cout << " - ";

    cout << " Region2 = ";
    if( GetRegion2(i) ) {
        cout << i;
    } else
        cout << " - ";
    cout << endl;

}

bool PiGLPlot::RingBuffer::GetRegion1( Interval& i ) const
{
    if( tail == head )
        return false;

    if( head > tail ) {
        i = Interval(tail,head);
    } else {
        i = Interval(tail,buffer.size()-1);
    }
    return true;
}

bool PiGLPlot::RingBuffer::GetRegion2(PiGLPlot::Interval &i) const
{
    if( head >= tail )
        return false;
    i = Interval(0,head);
    return true;
}
