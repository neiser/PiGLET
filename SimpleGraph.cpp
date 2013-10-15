#include "SimpleGraph.h"
#include "Window.h"

void SimpleGraph::set_labels(){

    for (int i = 0 ; i < _xlabels.size() ; ++i) delete _xlabels.at(i);
    for (int i = 0 ; i < _ylabels.size() ; ++i) delete _ylabels.at(i);
    _xlabels.clear();
    _ylabels.clear();
    _xlabels.reserve(1 + _xticks.size());
    _ylabels.reserve(1 + _yticks.size());
\
    for (int i = 0 ; i < _xticks.size() ; ++i ){
        _xlabels.push_back(new NumberLabel(this->_owner,Vector2()));
        _xlabels.at(i)->SetColor(kRed);
        _xlabels.at(i)->SetDigits(5);
        _xlabels.at(i)->SetPrec(1);
        _xlabels.at(i)->Set(_xticks.at(i).x);
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
    float dx = 1.;
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
    }
    return dx;
}


void SimpleGraph::UpdateTicks(){
    const float xlen = _blocklist.GetBackLenght();
    //calulate rough estimate how many ticks:
    const int ntx = _owner->XPixels() / 150;
    // replace with epics values:
    const float ylen = 2; // (maximum values of sin - function
    const int nty = _owner->YPixels() / 150;

    _xticks.clear();
    _xticks.reserve( ntx * 2);

    //get optimized distance close to calculated:
    float dx = dticks(xlen,ntx);

    vec2_t t;
    t.x = xlen / 2;
    int i = 0;
    while(t.x > -xlen / 2){
        t.x = xlen / 2 - ( i + 1 ) * dx;
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
    while(t.y > - ylen / 2){
        t.x = -xlen / 2;
        t.y = ylen / 2 - ( i +1 ) * dy;
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
