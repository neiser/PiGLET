#ifndef WIDGET_H
#define WIDGET_H

#include "BlockBuffer.h"
#include "Structs.h"
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

class SimpleGraph: public Widget {

private:
    PiGLPlot::BlockList _blocklist;
    std::vector<vec2_t> _xticks;
    std::vector<vec2_t> _yticks;
    float dXticks( const float& xlen, const int& target_nt );

public:
    SimpleGraph( Window* owner, const float backlength ): Widget(owner), _blocklist(backlength) { UpdateTicks();}

    void AddToBlockList( const vec2_t& p) {
        _blocklist.Add(p);
    }

    void UpdateTicks();

    void DrawTicks();
    void Draw();

    void SetNow( const float now ) { _blocklist.SetNow(now); }
    void SetBackLength( const float len ) { _blocklist.SetBackLength( len ); }

};

#endif // BLOCKBUFFER_H
