#ifndef SIMPLEGRAPH_H
#define SIMPLEGRAPH_H

#include "Widget.h"
#include "GLTools.h"
#include "BlockBuffer.h"
#include "NumberLabel.h"

#define NTICKSFULLX  6
#define NTICKSFULLY  5

class SimpleGraph: public Widget {

private:

    std::vector<vec2_t> _xticks;
    std::vector<vec2_t> _yticks;
    std::vector<NumberLabel*> _xlabels;
    std::vector<NumberLabel*> _ylabels;

    PiGLPlot::BlockList _blocklist;
    NumberLabel ValueDisplay;
    UnitBorderBox PlotArea;

    float dticks( const float& len, const int& target_nt );

    void set_labels();

public:
    Color TickColor;
    Color TickLabelColor;

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
