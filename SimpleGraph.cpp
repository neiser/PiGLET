#include "SimpleGraph.h"
#include "Window.h"
#include <cmath>

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


void SimpleGraph::UpdateTicks(){
    const float xlen = _blocklist.GetBackLenght();
    //calulate rough estimate how many ticks:
    const int ntx = ceil ( NTICKSFULLX *  _owner->XPixels() / GetWindowWidth()) ;
    // replace with epics values:
    const float ylen = 2; // (maximum values of sin - function
    const int nty = ceil( NTICKSFULLY *  _owner->YPixels() / GetWindowWidth());

    _xticks.clear();
    _xticks.reserve( ntx * 2);

    //get optimized distance close to calculated:
    float dx = dticks(xlen,ntx);

    vec2_t t;
    t.x = 0;
    int i = 0;
    while( true ){
        t.x = - ( i) * dx;
        if ( t.x < -xlen ) break;
        t.y = -ylen/2;
        _xticks.push_back(t);
        t.y = ylen/2;
        _xticks.push_back(t);
        i++;
    }

    _yticks.clear();
    _yticks.reserve( nty * 2 );
    float dy = dticks(ylen,nty);
    t.y = ylen / 2;
    i = 0;
    while(true){
        t.x = -xlen;
        t.y = ylen / 2 - ( i ) * dy;   // TODO: optimize startpoint!!
        if ( t.y < -ylen / 2) break;
        _yticks.push_back(t);
        t.x = 0;
        _yticks.push_back(t);
        i++;
    }
    set_labels();
}

void SimpleGraph::DrawTicks()
{
    glPushMatrix();

    glScalef( 2./ _blocklist.GetBackLenght(), 1 ,1);
    glTranslatef( _blocklist.GetBackLenght() / 2. , 0. , 0. );

    TickColor.Activate();

    glVertexPointer(2, GL_FLOAT, 0, _xticks.data());
    glDrawArrays(GL_LINES, 0, _xticks.size());
    glVertexPointer(2,GL_FLOAT, 0,_yticks.data());
    glDrawArrays(GL_LINES,0,_yticks.size());

    for ( size_t i = 0 ; i < _xlabels.size() ; ++i){
        glPushMatrix();
        glTranslatef(_xticks.at(i).x,-1.1,0);
        glScalef(0.15 * _blocklist.GetBackLenght() / 2,0.15,0.15);
        _xlabels.at(i)->Draw();
        glPopMatrix();
    }

    for ( size_t i = 0 ; i < _ylabels.size() ; ++i ){
        glPushMatrix();
        glTranslatef( 0.05 * _blocklist.GetBackLenght() ,_yticks.at(i).y, 1.);
        glScalef(0.15 * _blocklist.GetBackLenght() / 2 , 0.15 , 0.15);
        _ylabels.at(i)->Draw();
        glPopMatrix();
    }

    glPopMatrix();

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
