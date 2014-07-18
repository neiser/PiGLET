#include "TextLabel.h"
#include "TextRenderer.h"

using namespace std;


TextLabel::TextLabel( const Window* owner, const float x1, const float y1, const float x2, const float y2):
    Widget(owner), _color( dTextColor ), _rext(x1, y1, x2, y2)
{
}

TextLabel::~TextLabel()
{
}

void TextLabel::Draw() const
{

    
    const float rect_ratio = _rext.Width() / _rext.Height() ;
    const float text_ratio = _texture.GetAspectRatio()/GetWindowAspect();
    Rectangle   box(_rext);       // the actual drawing box, always smaller than the user defined text rectangle
    
    if( text_ratio >= rect_ratio ) {
        box= Rectangle( _rext.Center(), _rext.Width(), 1.0/text_ratio * _rext.Width() );
    } else {
        box= Rectangle( _rext.Center(), text_ratio * _rext.Height() , _rext.Height() );
    }
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    _color.Activate();
    _texture.Activate();
    
    box.Draw( GL_TRIANGLE_FAN );

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);   
    
    // uncomment for debugging aspect ratio stuff...
    //box.Draw( GL_LINE_LOOP );
    //_rext.Draw( GL_LINE_LOOP );
    
   
}

void TextLabel::SetText( const string &text )
{
    TextRenderer render;
    render.Text2Texture( _texture, text );
}
