#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <ostream>
#include <vector>
#include <string>

#include "Window.h"
#include "TextRenderer.h"

class WindowManager {
    
private:
    int _size_x;
    int _size_y;
    std::vector<Window*> _window_list;
    std::vector<int> _rows;
    Texture _tex;
    TextRenderer _render;

    std::string callbackRemoveAllWindows(const std::string& arg );
    std::string callbackAddPlotWindow( const std::string& arg );
    std::string callbackAddImageWindow(const std::string &arg);

    void alignWindows();    
public:

    WindowManager( const int dx = 1, const int dy = 1);

    const int& SizeX() const { return _size_x; }
    const int& SizeY() const { return _size_y; }
    int& SizeX() { return _size_x; }
    int& SizeY() { return _size_y; }
    const size_t NumWindows() { return _window_list.size();}

    std::string AddWindow( Window *win);

    // remove window number n; returns >0 if it fails
    int RemoveWindow( const size_t n );
    int RemoveWindow( const std::string& name );
    
    void Draw();



};

std::ostream& operator<<( std::ostream& stream, const WindowManager& wman );


#endif // INTERVAL_H
