#ifndef INTERVAL_H
#define INTERVAL_H

#include <ostream>



class Interval {
private:
    float _min;
    float _max;

public:

    Interval( const float min=0.0f, const float max=0.0f ): _min(min), _max(max) {}

    const float& Min() const { return _min; }
    const float& Max() const { return _max; }
    float& Min() { return _min; }
    float& Max() { return _max; }
    float Length() const { return _max - _min; }
    float Center() const { return (_min + _max) / 2.0f; }

    bool Disjoint( const Interval& i) const;

    bool Contains( const float x ) const { return (_min <= x) && (_max >= x); }

};

std::ostream& operator<<( std::ostream& stream, const Interval& i );




#endif // INTERVAL_H
