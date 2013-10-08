#include "Interval.h"


std::ostream& PiGLPlot::operator<<( std::ostream& stream, const Interval& i ) {
    stream << "[" << i.Min() << "," << i.Max() << "]";
    return stream;
}


bool PiGLPlot::Interval::Disjoint(const PiGLPlot::Interval &i) const
{
    return (Max() < i.Min()) || (i.Max() < Min());
}
