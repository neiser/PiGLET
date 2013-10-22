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

    /**
     * @brief Extend the interval to include x
     * @param x
     * @return false if x is already included
     */
    bool Extend( const float x ) {

        if( x > _max ) {
            _max = x;
            return true;
        } else if (x < _min ) {
            _min = x;
            return true;
        }

        return false;
    }

    bool Extend( const Interval& i ) {

        bool ret = false;

        if( i._max > _max ) {
            _max = i._max;
            ret = true;
        }

        if( i._min < _min ) {
            _min = i._min;
            ret = true;
        }

        return ret;
    }

    bool operator==( const Interval& i ) const {
        return (_min == i._min) && (_max == i._max);
    }

    bool operator!=(const Interval& i ) const {
        return ! ( (_min == i._min) && (_max == i._max));
    }

};

std::ostream& operator<<( std::ostream& stream, const Interval& i );




#endif // INTERVAL_H
