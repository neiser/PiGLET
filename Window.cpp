#include "Window.h"


std::ostream& operator<<( std::ostream& stream, const Window& win ) {
    stream << "[ " << win.Width() << " x " << win.Height() << " ]";
    return stream;
}


std::ostream& operator<<( std::ostream& stream, const PlotWindow& win ) {
    stream << "[ " << win.Width() << " x " << win.Height() << " ]: " << win.Xlabel() << " vs. " << win.Ylabel();
    return stream;
}
