#ifndef WIDGET_H
#define WIDGET_H

#include "BlockBuffer.h"
#include "Structs.h"

class Window;

class Widget{

protected:
    Window* _owner;
    float GetWindowAspect() const;

public:
    Widget( Window* owner): _owner(owner){}
    void Draw();

};

class SimpleGraph: public Widget{

private:
    PiGLPlot::BlockList _blocklist;

public:
    SimpleGraph( Window* owner, const PiGLPlot::BlockList&  blocklist ): Widget(owner), _blocklist(blocklist) {}

    void AddToBlockList( const vec2_t& p){
        _blocklist.Add(p);
    }

    void DrawTicks();
    void Draw();
    void SetNow( const float now ) { _blocklist.SetNow(now); }


};

#endif // BLOCKBUFFER_H
