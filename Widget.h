#ifndef WIDGET_H
#define WIDGET_H

#include "BlockBuffer.h"
#include "Window.h"

class Widget{

protected:
    Window* _owner;

public:
    Widget( Window* owner): _owner(owner){}
    void Draw();

};

class SimpleGraph: public Widget{

private:
    PiGLPlot::BlockList _blocklist;

public:
    SimpleGraph( Window* owner, const PiGLPlot::BlockList&  blocklist ): Widget(owner), _blocklist(blocklist) {}

    void DrawTicks();
    void Draw();


};

#endif // BLOCKBUFFER_H
