#include "SimpleGraph.h"
#include "Window.h"
#include <cmath>
#include <iostream>
#include <Sound.h>

using namespace std;

float SimpleGraph::GetXGlobal(const float x)
{
    return 1.0f + 2.0f * x  / _blocklist.XRange().Length();
}

float SimpleGraph::GetYGlobal(const float y)
{
    return 2.0f * (y  - _yrange.Center()) / _yrange.Length();
}

SimpleGraph::SimpleGraph( Window* owner, const float backlength ):
    Widget(owner),
    _blocklist(backlength),
    _yrange(),
    _yrange_manual(),
    _autorange(true),
    ValueDisplay(this->_owner),
    _prev_color(dTextColor),    
    _curr_color(dTextColor),        
    PlotArea( dPlotBackground, dPlotBorderColor),
    _minorAlarm(dMinorAlarm),
    _majorAlarm(dMajorAlarm),    
    TickColor(dPlotTicks),
    TickLabelColor(dPlotTickLabels),
    StartLineColor(dStartLineColor),
    enable_lastline(false)
{
    _time_since_noalarm.Start(-ALARM_DECAY_TIME);
    vec2_t init;
    init.x = 0./0.;
    init.y = 0./0.;
    _lastline[0] = init;
    ValueDisplay.SetDigits(10);
    UpdateTicks();
}

SimpleGraph::~SimpleGraph()
{
    DeleteTicks();
}

void SimpleGraph::AddToBlockList(const vec2_t &p)
{
    // check if we had a previous value,
    // then add it with the new x, but old y to
    // create a "stepped" plotting (instead of linear slope)    
    if(!isnan(_lastline[0].x)) {
        vec2_t p_old = p;
        p_old.y = _lastline[0].y;
        _blocklist.Add(p_old);
    }
    
    ValueDisplay.SetNumber(p.y);    
    _blocklist.Add(p);
    _lastline[0] = p;
    _lastline[1] = p;
    
    // trigger autorange calculation, if enabled
    SetAutoRange(_autorange);
}

void SimpleGraph::NewBlock()
{
    // start a new block and don't copy last
    // value = do not connect with a line to
    // previous
    _blocklist.NewBlock(false);
}

void SimpleGraph::Draw()
{
    // set the fading/blinking color of the ValueDisplay
    
    _time_since_noalarm.Stop();
    if(_time_since_noalarm.TimeElapsed()<ALARM_DECAY_TIME) {
        /*const double r = _time_since_noalarm.TimeElapsed()/ALARM_DECAY_TIME;
        ValueDisplay.SetColor(
                    Color::Interpolate(r,
                                       _prev_color,
                                       dTextColor
                                  ));*/
        if((int)(3*_time_since_noalarm.TimeElapsed()) % 2 == 0) {
            ValueDisplay.SetColor(_curr_color);
        }
        else {
            ValueDisplay.SetColor(_prev_color);
        }
    }
    else {
        ValueDisplay.SetColor(_curr_color);
    }
    
    //cout << _time_since_noalarm.TimeElapsed() << endl;
    
    
    glPushMatrix();
        const float scale_y = 0.72f;
        const float scale_x = 0.8f;
        glTranslatef(-.08, -.12, 0);
        glScalef(scale_x, scale_y, 1.0f);
        //glScalef(scale_x, 1.0f, 1.0f);

        PlotArea.Draw();
        DrawTicks();

      

        // limit draw area to plot area box
        glEnable(GL_STENCIL_TEST);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glStencilFunc(GL_NEVER, 1, 0xFF);
        glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);  // draw 1s on test fail (always)

        // draw stencil pattern
        glStencilMask(0xFF);
        glClear(GL_STENCIL_BUFFER_BIT);  // needs mask=0xFF

        PlotArea.Draw();

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glStencilMask(0x00);
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        // draw area is now limited
        
        
        _minorAlarm.Draw();
        _majorAlarm.Draw();
       
        glPushMatrix();

            // change to graph coordinates
            glScalef( 2.0f / _blocklist.XRange().Length(), 2.0f /  _yrange.Length(), 1.0f );
            glTranslatef(-_blocklist.XRange().Center(), -_yrange.Center(), 0.0f );

            _blocklist.Draw();

            if(enable_lastline) {
                StartLineColor.Activate();
                glVertexPointer(2,GL_FLOAT,0, _lastline);
                glDrawArrays(GL_LINES,0,2);
            }

        glPopMatrix();  // ed of graph coordinates

        // stop limiting draw area
        glDisable(GL_STENCIL_TEST);



        glPushMatrix();
            glTranslatef(-.7,.75,0);
            glScalef(.6,.6,.3);
            ValueDisplay.Draw();
        glPopMatrix();



    glPopMatrix();

}

void SimpleGraph::SetYRange(const Interval &yrange)
{
    
    if( yrange.Length() <= 0 ) {
        SetAutoRange(true);
        return;
    }
    
    _yrange = yrange;
    
    
    UpdateTicks();

    // update alarm levels yranges by explicitly setting them
    SetMinorAlarms(_minorAlarm.Levels());
    SetMajorAlarms(_majorAlarm.Levels());
}

void SimpleGraph::SetAutoRange(const bool autorange)
{
    _autorange = autorange;
    
    if( _autorange ) {
        Interval y = _blocklist.YRange();
        if( _yrange.Length() == 0 || y != _yrange ) {
            float scale = y.Length()>abs(y.Max()) ? y.Length() : abs(y.Max());
            float len = 0.1*scale;
            if(len <= 1.0) {
                len = 1.0;
            }
            y.Extend(y.Max()+len);
            y.Extend(y.Min()-len);
            SetYRange(y);
        }
    }
}


void SimpleGraph::SetYRangeMin(const double val)
{
    SetAutoRange(false);
    _yrange_manual = Interval(val, _yrange_manual.Max());
    SetYRange(_yrange_manual);    
}

void SimpleGraph::SetYRangeMax(const double val)
{
    SetAutoRange(false);
    _yrange_manual = Interval(_yrange_manual.Min(), val);    
    SetYRange(_yrange_manual);    
}

void SimpleGraph::SetMinorAlarms(const Interval &minoralarm )
{
    _minorAlarm.SetLevels( 
                minoralarm,
                Interval(
                    GetYGlobal(minoralarm.Min()),
                    GetYGlobal(minoralarm.Max())
                    )
                );
}

void SimpleGraph::SetMinorAlarmsMin(const double val)
{
    SetMinorAlarms(Interval(val, _minorAlarm.Levels().Max()));
}

void SimpleGraph::SetMinorAlarmsMax(const double val)
{
    SetMinorAlarms(Interval(_minorAlarm.Levels().Min(), val));
}

void SimpleGraph::SetMajorAlarms(const Interval &majoralarm)
{
    _majorAlarm.SetLevels( 
                majoralarm,
                Interval(
                    GetYGlobal(majoralarm.Min()),
                    GetYGlobal(majoralarm.Max())
                    )
                );
}

void SimpleGraph::SetMajorAlarmsMin(const double val)
{
    SetMajorAlarms(Interval(val, _majorAlarm.Levels().Max()));
}

void SimpleGraph::SetMajorAlarmsMax(const double val)
{
    SetMajorAlarms(Interval(_majorAlarm.Levels().Min(), val));
}


void SimpleGraph::SetPrecision(const unsigned short prec)
{
    ValueDisplay.SetPrec(prec);
}

void SimpleGraph::SetAlarm(const epicsAlarmSeverity serv )
{
    // disable fading by default
    _time_since_noalarm.Start(-ALARM_DECAY_TIME);    
    switch (serv ) {
    case epicsSevNone:
        _time_since_noalarm.Start();
        _prev_color = ValueDisplay.GetColor();
        _curr_color = dTextColor;
        break;
    case epicsSevMinor:
        Sound::I().Play("warning");        
        _curr_color = dMinorAlarm;
        break;
    case epicsSevMajor:
        Sound::I().Play("alert");        
        _curr_color = dMajorAlarm;
        break;
    default:
        // in most cases: epicsSevInvalid
        _curr_color = dInvalidAlarm;
        break;
    }
}

// --------- Ticks ----------

SimpleGraph::TickLabel::TickLabel(const Window *owner, const vec2_t &pos, const float v, const Color& color ):
    NumberLabel(owner),
    position(pos)
{
    SetColor( color );
    SetDrawBox(false);
    SetAlignRight(true);
    SetDigits(5);
    SetPrec(2);
}


void SimpleGraph::TickLabel::Draw() const
{
    glPushMatrix();
        glTranslatef( position.x, position.y, 0.0f );
        glScalef( .2f, .2f, .2f);
        NumberLabel::Draw();
    glPopMatrix();
}


void SimpleGraph::DrawTicks() const
{

     //Draw all tick lines
    TickColor.Activate();
    glVertexPointer(2, GL_FLOAT, 0, _ticks.data());
    glDrawArrays(GL_LINES, 0, _ticks.size());

    // draw all labels
    labellist::const_iterator i;
    for( i=_labels.begin(); i!=_labels.end(); ++i)
        (*i)->Draw();

}

void SimpleGraph::UpdateTicks() {

    DeleteTicks();

    //calulate rough estimate how many ticks:
    int ntx = ceil ( NTICKSFULLX *  _owner->XPixels() / GetWindowWidth());
    float dx = _blocklist.XRange().Length() / ntx;
    dx = roundX(dx);
    ntx = floor( _blocklist.XRange().Length() / dx ) +1;

    for( int i=0; i<ntx; ++i ) {
        float x = 0 - i * dx;
        AddXTick( x );
    }
      
    int nty = ceil (NTICKSFULLY * _owner->YPixels() / GetWindowHeight());
    float dy = _yrange.Length() / nty;
    float dy_r = roundX(dy);
    float nty_r = round( _yrange.Length() / dy_r );
    
    // since the rounded ystart might be outside _yrange already
    // we scan with nty a little bit more around...
    const float ystart = roundX( _yrange.Center() - nty*dy/2 ) ;
    for( int i=-2; i<nty_r+2; ++i ) {
        float y = ystart + i * dy_r;
        if(_yrange.Contains(y))
            AddYTick( y );
    }
}

void SimpleGraph::DeleteTicks()
{
    labellist::iterator i;
    for( i=_labels.begin(); i != _labels.end(); ++i)
        delete (*i);
    _labels.clear();
    _ticks.clear();
}


float SimpleGraph::roundX(float x) {
    if( x==0 || !isfinite(x) )
        return 0;
    float m=1;
    if (abs(x) >= 1 ) {
        while( abs(x) > 10 ) {
            x /= 10.0;
            m *= 10.0;
        }
    } else {
        while( abs(x) < 1 ) {
            x *= 10.0;
            m /= 10.0;
        }
    }
    x = round(x) * m;

    return x;
}

void SimpleGraph::AddXTick(const float x) {
    vec2_t t;
    t.x = GetXGlobal(x);
    t.y =   1.0f;
    _ticks.push_back(t);
    t.y = - 1.0f;
    _ticks.push_back(t);

    t.y -= .1;

    TickLabel* label = new TickLabel( _owner, t, x );
    label->SetTime(x);

    _labels.push_back(label);

}

void SimpleGraph::AddYTick(const float y) {
    vec2_t t;
    t.y = GetYGlobal(y);
    t.x =  -1.0f;
    _ticks.push_back(t);
    t.x =   1.0f;
    _ticks.push_back(t);

    t.x += .2;

    TickLabel* label = new TickLabel( _owner, t, y );
    label->SetNumber(y);

    _labels.push_back(label);
}


SimpleGraph::AlarmLevels::AlarmLevels(const Color &color):
    AlarmColor(color)
{
}

void SimpleGraph::AlarmLevels::Draw()
{
    if(_levels.Length()==0)
        return;
    AlarmColor.Activate();
    glLineWidth(1.0f);
    glVertexPointer(2, GL_FLOAT, 0, _lines.data());
    glDrawArrays(GL_LINES, 0, _lines.size());
}

void SimpleGraph::AlarmLevels::SetLevels(const Interval& levels , const Interval &draw)
{
    _levels = levels;

    if( !isfinite(_levels.Min()) )
        _levels.Min() = 0;

    if( !isfinite(_levels.Max()) )
        _levels.Max() = 0;

    _draw_levels = draw;
    Update();

}


void SimpleGraph::AlarmLevels::Clear()
{
    _lines.clear();
}


void SimpleGraph::AlarmLevels::Update()
{

    Clear();
    vec2_t t;
    t.x = -1.0f;
    t.y = _draw_levels.Min();
    _lines.push_back(t);
    t.x=1.0f;
    _lines.push_back(t);

    t.x = -1.0f;
    t.y = _draw_levels.Max();
    _lines.push_back(t);
    t.x=1.0f;
    _lines.push_back(t);
}
