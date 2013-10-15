#include "NumberLabel.h"
#include <sstream>
#include <string>
#include <iomanip>
#include "TextRenderer.h"

using namespace std;

NumberLabel::NumberLabel( Window* owner, const Vector2& pos ):
    Widget(owner),
    _color(dTextColor),
    _prec(2),
    _digits(7),
    _align_right(true)
{
    _maketextures();
}

NumberLabel::~NumberLabel()
{
    --_num_objetcs;

    if( _num_objetcs == 0) {
        glDeleteTextures(NUMBERLABEL_NUM_TEX, _textures);
    }
}

vec2_t NumberLabel::_texcoords[4];

void NumberLabel::Draw() const
{



    unsigned char digits = 0;
    if( _align_right ) {
        digits = _digits;
    } else {
        digits = _digtex.size();
    }

    if( digits > 0 ) {

        glPushMatrix();

        glScalef(1 , _texratio , 1.0f );

        Box.Draw();

        glScalef(2.0f / (digits * 2.0),1,1);

        glTranslatef( (digits-1), 0.0, 0.0);

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        glTexCoordPointer(2, GL_FLOAT, 0, _texcoords);


        for( int p=0;p<_digtex.size();++p ) {

            glBindTexture(GL_TEXTURE_2D, _digtex[p]);
            Rectangle::unit.Draw( GL_TRIANGLE_FAN );
            glTranslatef( -2.0f, 0.0f, 0.0f );

        }

        glPopMatrix();

        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    }

}

void NumberLabel::Set(const float v)
{
    // generate text string from number
    stringstream stream;
    stream << fixed << setprecision(_prec) << v;
    string s = stream.str();

    _digtex.resize(s.size());

    for( int p=0; p< s.size(); ++p) {

        const char& c = s[p];
        unsigned char texnum;

        if ( c>='0' && c<= '9') {
            texnum = c - '0';
        } else {
            if( c == '.' )
                texnum = 10;
            else if( c == '-')
                texnum = 11;
            else if( c == 'E' || c == 'e' )
                texnum = 12;
            else
                texnum = 13;
        }
         _digtex[s.size() - p-1] = _textures[texnum];

    }
}

unsigned int NumberLabel::_num_objetcs = 0;
GLuint NumberLabel::_textures[NUMBERLABEL_NUM_TEX];
float NumberLabel::_texratio = 1.0;

void NumberLabel::_maketextures()
{
    float maxw =0;
    float maxh =0;
    float w,h;
    if(_num_objetcs==0) {

        _num_objetcs++;

        glGenTextures(NUMBERLABEL_NUM_TEX, _textures);

        const char chars[] = NUMBERLABEL_CHARS;

        for( int i=0; i<NUMBERLABEL_NUM_TEX; ++i ) {
            stringstream s;
            s << chars[i];

            TextRenderer::I().Text2Texture2(_textures[i], s.str(),64,64,w,h);
            if(w>maxw)
                maxw=w;
            if(h>maxh)
                maxh=h;
        }

        _texcoords[0].x = 0;
        _texcoords[0].y = maxh;

        _texcoords[1].x = 0;
        _texcoords[1].y = 0;

        _texcoords[2].x = maxw;
        _texcoords[2].y = 0;

        _texcoords[3].x = maxw;
        _texcoords[3].y = maxh;

        _texratio = maxw / maxh;

    }

}
