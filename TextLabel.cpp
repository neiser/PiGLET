#include "TextLabel.h"
#include "TextRenderer.h"

using namespace std;


TextLabel::TextLabel( const Window* owner, const float x1, const float y1, const float x2, const float y2):
    Widget(owner), _color( dTextColor ), _rext(x1, y1, x2, y2),  _box(_rext)
{
}

TextLabel::~TextLabel()
{
}

void TextLabel::Draw() const
{

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    _color.Activate();

    _texture.Activate();

    glPushMatrix();

        glScalef( 1.0f / GetWindowAspect(), 1.0f, 1.0f );
        _box.Draw( GL_TRIANGLE_FAN );

    glPopMatrix();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}

void TextLabel::SetText( const string &text )
{
    TextRenderer::I().Text2Texture( _texture, text );

    const float rect_ratio = _rext.Width() / _rext.Height();

    if( _texture.GetAspectRatio() >= rect_ratio ) {
        _box= Rectangle( _rext.Center(), _rext.Width(), 1.0/_texture.GetAspectRatio() * _rext.Width() );
    } else {
        _box= Rectangle( _rext.Center(), _texture.GetAspectRatio() * _rext.Height(), _rext.Height() );
    }

}
