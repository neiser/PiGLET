#ifndef SIMPLEGRAPH_H
#define SIMPLEGRAPH_H

#include "Widget.h"
#include "GLTools.h"
#include "BlockBuffer.h"
#include "NumberLabel.h"


class SimpleGraph: public Widget {

private:
    PiGLPlot::BlockList _blocklist;
    std::vector<vec2_t> _xticks;
    std::vector<vec2_t> _yticks;
    float dXticks( const float& len, const int& target_nt );
    NumberLabel ValueDisplay;

public:
    Color TickColor;
    SimpleGraph( Window* owner, const float backlength );

    void AddToBlockList( const vec2_t& p) {
        _blocklist.Add(p);
        ValueDisplay.Set(p.y);
    }

    void UpdateTicks();

    void DrawTicks();
    void Draw();

    void SetNow( const float now ) { _blocklist.SetNow(now); }
    void SetBackLength( const float len ) { _blocklist.SetBackLength( len ); }

};
#endif
