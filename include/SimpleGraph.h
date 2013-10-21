#ifndef SIMPLEGRAPH_H
#define SIMPLEGRAPH_H

#include "Widget.h"
#include "GLTools.h"
#include "BlockBuffer.h"
#include "NumberLabel.h"
#include "Interval.h"
#include "alarm.h"
#include <list>

#define NTICKSFULLX  6
#define NTICKSFULLY  5

class SimpleGraph: public Widget {

private:

    // ------ Tick System -------

    class TickLabel: public NumberLabel {
    public:
        vec2_t position;

        TickLabel(const Window* owner, const vec2_t& pos, const float v , const Color &color = dPlotTickLabels);
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

    /**
     * @brief The AlarmLevels class
     * @todo This whole solution is crappy... These lines should be drawn
     *       while in the coord sys of the blocklist. But then, the lines
     *       have to be moved in x every frame.
     *       Like this we have to update the y position every time we change the
     *       y range.
     */
    class AlarmLevels {
    private:
        Interval _levels;
        Interval _draw_levels;
        std::vector<vec2_t> _lines;

    public:
        Color AlarmColor;

        AlarmLevels( const Color& color=dMinorAlarm);
        void Draw() const;
        void SetLevels( const Interval& levels, const Interval& draw );
        void Clear();
        void Update();
        const Interval& Levels() const { return _levels; }

    };

    AlarmLevels _minorAlarm;
    AlarmLevels _majorAlarm;

    mutable vec2_t _lastline[2];


public:
    Color TickColor;
    Color TickLabelColor;   //note: does not change color on screen when changed!
    Color StartLineColor;

    bool  enable_lastline;

    SimpleGraph( Window* owner, const float backlength );
    virtual ~SimpleGraph();

    void AddToBlockList( const vec2_t& p) {
        _blocklist.Add(p);
        ValueDisplay.SetNumber(p.y);
        _lastline[0] = p;
        _lastline[1] = p;
    }

    void UpdateTicks();

    void DrawTicks() const;
    void Draw() const;

    void SetNow( const float now ) { _blocklist.SetNow(now); _lastline[1].x=now; }
    void SetBackLength( const float len ) { _blocklist.SetBackLength( len ); UpdateTicks(); }
    void SetYRange( const Interval& yrange );
    void SetMinorAlarms( const Interval& minoralarm );
    void SetMajorAlarms( const Interval& majoralarm );
    void SetPrecision(const unsigned char prec);

    /**
     * @brief Set the alarm state
     * @param serv Severity of the alarm
     */
    void SetAlarm(const epicsAlarmSeverity serv );

};
#endif
