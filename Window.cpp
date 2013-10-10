#include "Window.h"

void PlotWindow::Draw(){
    rect.Draw( GL_LINE_LOOP );
}

std::ostream& operator<<( std::ostream& stream, const Window& win ) {
    stream << "[ " << win.Width() << " x " << win.Height() << " ]";
    return stream;
}


std::ostream& operator<<( std::ostream& stream, const PlotWindow& win ) {
    stream << "[ " << win.Width() << " x " << win.Height() << " ]: " << win.Xlabel() << " vs. " << win.Ylabel();
    return stream;
}
