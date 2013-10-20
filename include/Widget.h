#ifndef WIDGET_H
#define WIDGET_H

#include <vector>

class Window;

class Widget {

protected:
    const Window* _owner;
    float GetWindowAspect() const;

public:
    Widget( const Window* owner ): _owner(owner){}
    void Draw();

};

#endif // BLOCKBUFFER_H
