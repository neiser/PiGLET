#include "SimpleGraph.h"
#include "Window.h"
#include <cmath>
#include <iostream>

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
    _autorange(true),
    ValueDisplay(this->_owner),
    PlotArea( dPlotBackground, dPlotBorderColor),
    _minorAlarm(dMinorAlarm),
    _majorAlarm(dMajorAlarm),
    TickColor(dPlotTicks),
    TickLabelColor(dPlotTickLabels),
    StartLineColor(dStartLineColor),
    enable_lastline(true)
{
    ValueDisplay.SetDigits(10);
    UpdateTicks();
}

SimpleGraph::~SimpleGraph()
{
    DeleteTicks();
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

    glPushMatrix();

        glTranslatef(-.08, 0,0);
        glScalef(0.8f,0.8f,0.8f);


        PlotArea.Draw();
        DrawTicks();

        _minorAlarm.Draw();
        _majorAlarm.Draw();

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



        glPushMatrix();

            // change to graph coordinates
            glScalef( 2.0f / _blocklist.XRange().Length(), 2.0f / _yrange.Length(), 1.0f );
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
            glTranslatef(-.7,.85,0);
            glScalef(.5,.3,.3);
            ValueDisplay.Draw();
        glPopMatrix();



    glPopMatrix();

}

void SimpleGraph::SetYRange(const Interval &yrange)
{
    if( _yrange.Length() < 1.0f ) {
        _yrange = Interval( _yrange.Min() - .5f, _yrange.Max() + .5f);
    }

    _yrange = yrange;

    UpdateTicks();

    _minorAlarm.SetLevels( _minorAlarm.Levels(),
                Interval(
                    GetYGlobal(_minorAlarm.Levels().Min()),
                    GetYGlobal(_minorAlarm.Levels().Max())
                    )
                );

    _majorAlarm.SetLevels( _majorAlarm.Levels(),
                Interval(
                    GetYGlobal(_majorAlarm.Levels().Min()),
                    GetYGlobal(_majorAlarm.Levels().Max())
                    )
                );

}

void SimpleGraph::SetMinorAlarms(const Interval &minoralarm )
{
    _minorAlarm.SetLevels( minoralarm,
                Interval(
                    GetYGlobal(minoralarm.Min()),
                    GetYGlobal(minoralarm.Max())
                    )
                );
}

void SimpleGraph::SetMajorAlarms(const Interval &majoralarm)
{
    _majorAlarm.SetLevels( majoralarm,
                Interval(
                    GetYGlobal(majoralarm.Min()),
                    GetYGlobal(majoralarm.Max())
                    )
                           );
}

void SimpleGraph::SetPrecision(const unsigned char prec)
{
    ValueDisplay.SetPrec(prec);
}

void SimpleGraph::SetAlarm(const epicsAlarmSeverity serv )
{
    switch (serv ) {
    case epicsSevNone:
        ValueDisplay.SetColor(dTextColor);
        break;
    case epicsSevMinor:
        ValueDisplay.SetColor(dMinorAlarm);
        break;
    case epicsSevMajor:
        ValueDisplay.SetColor(dMajorAlarm);
        break;
    default:
        ValueDisplay.SetColor(dInvalidAlarm);
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
    SetPrec(1);
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

    int nty = ceil ( NTICKSFULLY *  _owner->YPixels() / GetWindowHeight());
    float dy = _yrange.Length() / nty;
    dy = roundX(dy);
    nty = round( _yrange.Length() / dy ) ;
    
    const float ystart = roundX( _yrange.Center() ) - (nty/2)*dy;
    for( int i=0; i<nty; ++i ) {
        float y = ystart + i * dy;
        
        if(_yrange.Contains(y))
            AddYTick( y );
    }

 //   if( _blocklist.YRange().Contains(0.0f) ) {
 //       AddYTick(0.0f);
 //   }

}

void SimpleGraph::DeleteTicks()
{
    labellist::iterator i;
    for( i=_labels.begin(); i != _labels.end(); ++i)
        delete (*i);
    _labels.clear();
    _ticks.clear();
}


float SimpleGraph::roundX( float x ) {
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
