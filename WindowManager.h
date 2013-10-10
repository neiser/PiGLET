#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <ostream>
#include <vector>

#include "Window.h"
#include "Structs.h"


class WindowManager {
private:
    int _size_x;
    int _size_y;
    std::vector<Window*> _window_list;

public:

    WindowManager( const int dx, const int dy): _size_x(dx), _size_y(dy) {}

    const int& SizeX() const { return _size_x; }
    const int& SizeY() const { return _size_y; }
    int& SizeX() { return _size_x; }
    int& SizeY() { return _size_y; }
    const int NumWindows() { return _window_list.size();}

    int AddWindow( Window *win){ _window_list.push_back(win);  return NumWindows(); }

    void Draw();



};

std::ostream& operator<<( std::ostream& stream, const WindowManager& wman );


#endif // INTERVAL_H
