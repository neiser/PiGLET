#include "SimpleGraph.h"
#include "Window.h"
#include <cmath>

using namespace std;

void SimpleGraph::set_labels(){

    for (int i = 0 ; i < _xlabels.size() ; ++i) delete _xlabels.at(i);
    for (int i = 0 ; i < _ylabels.size() ; ++i) delete _ylabels.at(i);
    _xlabels.clear();
    _ylabels.clear();
    _xlabels.reserve(_xticks.size());
    _ylabels.reserve(_yticks.size());
\
    for (int i = 0 ; i < _xticks.size() ; ++i ){
        _xlabels.push_back(new NumberLabel(this->_owner));
        _xlabels.at(i)->SetDrawBox(false);
        _xlabels.at(i)->SetAlignRight(false);
        _xlabels.at(i)->SetColor(kRed);
        _xlabels.at(i)->SetDigits(5);
        _xlabels.at(i)->SetPrec(1);
        _xlabels.at(i)->Set(_xticks.at(i).x);
    }

    for ( int i = 0 ; i < _yticks.size() ; ++i ){
        _ylabels.push_back(new NumberLabel(this->_owner));
        _ylabels.at(i)->SetDrawBox(false);
        _ylabels.at(i)->SetAlignRight(true);
        _ylabels.at(i)->SetColor(kRed);
        _ylabels.at(i)->SetDigits(5);
        _ylabels.at(i)->SetPrec(1);
        _ylabels.at(i)->Set(_yticks.at(i).y);
    }

}

SimpleGraph::SimpleGraph( Window* owner, const float backlength ):
    Widget(owner),
    _blocklist(backlength),
    TickColor(dPlotTicks),
    ValueDisplay(this->_owner)
{
    UpdateTicks();
}

float SimpleGraph::dticks( const float& len, const int& nt ){
 /*   float dx = 1.;
    if ( len / dx > nt){
        while ( true ){
            dx*=2;
            if ( len / dx <= nt ) return dx;
            dx*=5./2;
            if ( len / dx <= nt ) return dx;
            dx*= 20./ 5.;
            if ( len / dx <= nt ) return dx;
        }
    }
    while ( true ){
      dx*=0.5;
      if ( len / dx > nt ) return dx;
      dx*=2./5;
      if ( len / dx > nt ) return dx;
      dx*= 5. / 20;
      if ( len / dx > nt ) return dx;
    }*/
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
    const int ntx = ceil ( 6 *  _owner->XPixels() / DEFAULT_WINDOW_WIDTH) ;
    // replace with epics values:
    const float ylen = 2; // (maximum values of sin - function
    const int nty = ceil( 4 *  _owner->YPixels() / DEFAULT_WINDOW_WIDTH );

    _xticks.clear();
    _xticks.reserve( ntx * 2);

    //get optimized distance close to calculated:
    float dx = dticks(xlen,ntx);

    vec2_t t;
    t.x = xlen / 2;
    int i = 0;
    while( true ){
        t.x = xlen / 2 - ( i) * dx;
        if ( t.x < -xlen / 2 ) break;
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
        t.x = -xlen / 2;
        t.y = ylen / 2 - ( i ) * dy;
        if ( t.y < -ylen / 2) break;
        _yticks.push_back(t);
        t.x = xlen / 2;
        _yticks.push_back(t);
        i++;
    }
    set_labels();
}

void SimpleGraph::DrawTicks()
{
    glPushMatrix();

    glScalef( 2.0 / _blocklist.GetBackLenght(), 1 ,1);
    TickColor.Activate();

    glVertexPointer(2, GL_FLOAT, 0, _xticks.data());
    glDrawArrays(GL_LINES, 0, _xticks.size());
    glVertexPointer(2,GL_FLOAT, 0,_yticks.data());
    glDrawArrays(GL_LINES,0,_yticks.size());

    for ( int i = 0 ; i < _xlabels.size() ; ++i){
        glPushMatrix();
        glTranslatef(_xticks.at(i).x,-1.1,0);
        glScalef(0.15 * _blocklist.GetBackLenght() / 2,0.15,0.15);
        _xlabels.at(i)->Draw();
        glPopMatrix();
    }

    for ( int i = 0 ; i < _ylabels.size() ; ++i ){
        glPushMatrix();
        glTranslatef( -1.1 * _blocklist.GetBackLenght() / 2.,_yticks.at(i).y, 1.);
        glScalef(0.15 * _blocklist.GetBackLenght() / 2 , 0.15 , 0.15);
        _ylabels.at(i)->Draw();
        glPopMatrix();
    }

    glPopMatrix();

}

void SimpleGraph::Draw()
{
    DrawTicks();
    _blocklist.Draw();


    glPushMatrix();
    // Position the value label (pure guessing)
    glTranslatef(-.7,.85,0);
    glScalef(.3,.3,.3);
    ValueDisplay.Draw();
    glPopMatrix();
}
