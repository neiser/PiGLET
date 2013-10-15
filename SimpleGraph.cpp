#include "SimpleGraph.h"
#include "Window.h"

SimpleGraph::SimpleGraph( Window* owner, const float backlength ):
    Widget(owner),
    _blocklist(backlength),
    TickColor(dPlotTicks),
    ValueDisplay(this->_owner, Vector2(0,0))
{
    UpdateTicks();
}

float SimpleGraph::dXticks( const float& len, const int& nt ){
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
    float dx = dXticks(xlen,ntx);

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
    float dy = dXticks(ylen,nty);
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
