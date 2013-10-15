#ifndef WIDGET_H
#define WIDGET_H

#include <vector>

class Window;

class Widget{

protected:
    Window* _owner;
    float GetWindowAspect() const;

public:
    Widget( Window* owner): _owner(owner){}
    void Draw();

};

#endif // BLOCKBUFFER_H
