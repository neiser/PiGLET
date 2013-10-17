#ifndef SIMPLEGRAPH_H
#define SIMPLEGRAPH_H

#include "Widget.h"
#include "GLTools.h"
#include "BlockBuffer.h"
#include "NumberLabel.h"
#include "Interval.h"
#include <list>

#define NTICKSFULLX  6
#define NTICKSFULLY  5

class SimpleGraph: public Widget {

private:

    // ------ Tick System -------

    class TickLabel: public NumberLabel {
    public:
        vec2_t position;

        TickLabel(const Window* owner, const vec2_t& pos, const float v , const Color &color = dPlotTicks);
        virtual ~TickLabel() {}
        void Draw() const;
    };

    typedef std::list<TickLabel*> labellist;
    labellist _labels;

    std::vector<vec2_t> _ticks;

    void DeleteTicks();
    float GetXGlobal( const float x);
    float GetYGlobal( const float y);
    void AddXTick( const float x);
    void AddYTick( const float y);

    // -------------------------

    BlockList _blocklist;
    NumberLabel ValueDisplay;
    UnitBorderBox PlotArea;

    static float roundX( float x);

public:
    Color TickColor;
    Color TickLabelColor;   //note: does not change color on screen when changed!

    SimpleGraph( Window* owner, const float backlength );
    virtual ~SimpleGraph();

    void AddToBlockList( const vec2_t& p) {
        _blocklist.Add(p);
        ValueDisplay.Set(p.y);
    }

    void UpdateTicks();

    void DrawTicks() const;
    void Draw() const;

    void SetNow( const float now ) { _blocklist.SetNow(now); }
    void SetBackLength( const float len ) { _blocklist.SetBackLength( len ); UpdateTicks(); }
    void SetYRange( const Interval& yrange );

};
#endif
