#include "Interval.h"


std::ostream& operator<<( std::ostream& stream, const Interval& i ) {
    stream << "[" << i.Min() << "," << i.Max() << "]";
    return stream;
}


bool Interval::Disjoint(const Interval &i) const
{
    return (Max() < i.Min()) || (i.Max() < Min());
}
