#include "TextLabel.h"
#include "TextRenderer.h"

using namespace std;


TextLabel::TextLabel( const Window* owner, const float x1, const float y1, const float x2, const float y2):
    Widget(owner), _rext(x1, y1, x2, y2), _color( dTextColor ),_box(_rext)
{
    glGenTextures(1,&_texture);
}

TextLabel::~TextLabel()
{
    glDeleteTextures(1, &_texture);
}

void TextLabel::Draw() const
{

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    _color.Activate();

    glTexCoordPointer(2, GL_FLOAT, 0, _texcoords);

    glBindTexture(GL_TEXTURE_2D, _texture);

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
    float w,h, tex_ratio;
    TextRenderer::I().Text2Texture3(_texture, text, w, h, tex_ratio);

    const float rect_ratio = _rext.Width() / _rext.Height();

    if( tex_ratio >= rect_ratio ) {
        _box= Rectangle( _rext.Center(), _rext.Width(), 1.0/tex_ratio * _rext.Width() );
    } else {
        _box= Rectangle( _rext.Center(), tex_ratio * _rext.Height(), _rext.Height() );
    }

    _texcoords[0].x = 0;
    _texcoords[0].y = h;

    _texcoords[1].x = 0;
    _texcoords[1].y = 0;

    _texcoords[2].x = w;
    _texcoords[2].y = 0;

    _texcoords[3].x = w;
    _texcoords[3].y = h;

}
