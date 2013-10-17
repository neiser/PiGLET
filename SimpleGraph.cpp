#include "SimpleGraph.h"
#include "Window.h"
#include <cmath>
#include <iostream>

using namespace std;

void SimpleGraph::set_labels(){

    for (size_t i = 0 ; i < _xlabels.size() ; ++i) delete _xlabels.at(i);
    for (size_t i = 0 ; i < _ylabels.size() ; ++i) delete _ylabels.at(i);
    _xlabels.clear();
    _ylabels.clear();
    _xlabels.reserve(_xticks.size());
    _ylabels.reserve(_yticks.size());
\
    for (size_t i = 0 ; i < _xticks.size() ; ++i ){
        _xlabels.push_back(new NumberLabel(this->_owner));
        _xlabels.at(i)->SetColor(TickLabelColor);
        _xlabels.at(i)->SetDrawBox(false);
        _xlabels.at(i)->SetAlignRight(true);
        _xlabels.at(i)->SetDigits(5);
        _xlabels.at(i)->SetPrec(1);
        _xlabels.at(i)->Set(_xticks.at(i).x);
    }

    for ( size_t i = 0 ; i < _yticks.size() ; ++i ){
        _ylabels.push_back(new NumberLabel(this->_owner));
        _ylabels.at(i)->SetColor(TickLabelColor);
        _ylabels.at(i)->SetDrawBox(false);
        _ylabels.at(i)->SetAlignRight(true);
        _ylabels.at(i)->SetDigits(5);
        _ylabels.at(i)->SetPrec(1);
        _ylabels.at(i)->Set(_yticks.at(i).y);
    }

}

float SimpleGraph::GetXGlobal(const float x)
{
    return 1.0f + 2.0f * x  / _blocklist.XRange().Length();
}

float SimpleGraph::GetYGlobal(const float y)
{
    return 2.0f * (y  - _blocklist.YRange().Center()) / _blocklist.YRange().Length();
}

SimpleGraph::SimpleGraph( Window* owner, const float backlength ):
    Widget(owner),
    _blocklist(backlength),
    ValueDisplay(this->_owner),
    PlotArea( dPlotBackground, dPlotBorderColor),
    TickColor(dPlotTicks),
    TickLabelColor(dPlotTickLabels)
{
    UpdateTicks();
}

float SimpleGraph::dticks( const float& len, const int& nt ){
    const float finedx = len / nt;
    float dx = 1;

    while ( dx < finedx ){
        float dx_old = dx;
        for (int i = 0 ; i < 10 ; ++i ){
            dx+= 0.1 * dx_old;
            if ( dx > finedx ) return dx;
        }
    }
    while ( dx >= finedx){
        float dx_old = dx;
        for ( int i = 0 ; i < 10 ; ++i ){
            dx-= 0.1 * dx_old;
            if ( dx < finedx ) return dx;
        }
    }

    return len / nt;
}

float roundX( float x ) {
    if( x==0 )
        return 0;
    float m=1;
    if (x >= 1 ) {
        while( x > 10 ) {
            x /= 10.0;
            m *= 10.0;
        }
    } else {
        while( x < 1 ) {
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
    _xticks.push_back(t);
    t.y = - 1.0f;
    _xticks.push_back(t);

    NumberLabel* label = new NumberLabel( _owner );
    label->Set( x );
    label->SetColor(TickLabelColor);
    label->SetDrawBox(false);
    label->SetAlignRight(true);
    label->SetDigits(5);
    label->SetPrec(1);
    _xlabels.push_back(label);
    cout << "  add at x=" << x << "n=" << _xticks.size() << endl;
}

void SimpleGraph::AddYTick(const float y) {
    vec2_t t;
    t.y = GetYGlobal(y);
    t.x =   1.0f;
    _yticks.push_back(t);
    t.x = - 1.0f;
    _yticks.push_back(t);
    NumberLabel* label = new NumberLabel( _owner );
    label->Set( y );
    label->SetColor(TickLabelColor);
    label->SetDrawBox(false);
    label->SetAlignRight(true);
    label->SetDigits(5);
    label->SetPrec(1);
    _ylabels.push_back(label);
    cout << "  add at y=" << y << "n=" << _yticks.size() << endl;
}

void SimpleGraph::UpdateTicks(){

    //calulate rough estimate how many ticks:
    int ntx = ceil ( NTICKSFULLX *  _owner->XPixels() / GetWindowWidth()) ;
    float dx = _blocklist.XRange().Length() / ntx;
    dx = roundX(dx);
    ntx = floor( _blocklist.XRange().Length() / dx ) +1;

    _xticks.clear();

    for (size_t i = 0 ; i < _xlabels.size() ; ++i) delete _xlabels.at(i);
    _xlabels.clear();

    _xticks.reserve( ntx *2 );
    _xlabels.reserve( ntx );

    cout << "X: n=" << ntx << " dx=" << dx << endl;

    for( int i=0; i<ntx; ++i ) {
        float x = 0 - i * dx;
        AddXTick( x );
    }

}

void SimpleGraph::DrawTicks()
{
    TickColor.Activate();

    glVertexPointer(2, GL_FLOAT, 0, _xticks.data());
    glDrawArrays(GL_LINES, 0, _xticks.size());

    glVertexPointer(2, GL_FLOAT, 0, _yticks.data());
    glDrawArrays(GL_LINES, 0, _yticks.size());

    for( size_t i=0; i < _xlabels.size(); ++i ) {
        glPushMatrix();
        glTranslatef( _xticks[2*i].x, -1.1,0.0);
        glScalef(.2,.2,.2);
        _xlabels[i]->Draw();
        glPopMatrix();
    }


}

void SimpleGraph::Draw()
{


    glPushMatrix();

        glScalef(0.8f,0.8f,0.8f);

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

        PlotArea.Draw();

        _blocklist.Draw();

        // stop limiting draw area
        glDisable(GL_STENCIL_TEST);

        DrawTicks();

        glPushMatrix();
            glTranslatef(-.7,.85,0);
            glScalef(.3,.3,.3);
            ValueDisplay.Draw();
        glPopMatrix();


    glPopMatrix();

}

void SimpleGraph::SetYRange(const PiGLPlot::Interval &yrange)
{
    _blocklist.SetYRange( yrange );
}

