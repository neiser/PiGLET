#include "NumberLabel.h"
#include <sstream>
#include <string>
#include <iomanip>
#include "TextRenderer.h"
#include <algorithm>

#include <iostream>

using namespace std;

NumberLabel::NumberLabel(const Window *owner ):
    Widget(owner),
    _color(dTextColor),
    _prec(2),
    _digits(7),
    _align_right(true),
    _draw_box(true)
{
    _maketextures();
}

NumberLabel::~NumberLabel()
{
    --_num_objetcs;

    if( _num_objetcs == 0) {
        delete [] _textures;
    }
}

vec2_t NumberLabel::_texcoords[4];

void NumberLabel::Draw() const
{
    unsigned char digits = 0;
    if( _align_right ) {
        digits = max((int)_digits, (int)_digtex.size());
    } else {
        digits = _digtex.size();
    }

    if( digits > 0 ) {

        glPushMatrix();

        glScalef( 1.0f / GetWindowAspect() , _textures[0].GetAspectRatio(), 1.0f );

        if (_draw_box)
            Box.Draw();

        glScalef(2.0f / (digits * 2.0f), 1.0f, 1.0f );

        glTranslatef( (digits-1), 0.0f, 0.0f);

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
       // glTexCoordPointer(2, GL_FLOAT, 0, _texcoords);

        _color.Activate();

        for( int p=0; p<_digtex.size(); ++p ) {

            //glBindTexture(GL_TEXTURE_2D, _digtex[p]);
            _textures[ _digtex[p] ].Activate();
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
         _digtex[s.size() - p-1] = texnum;

    }
}

unsigned int NumberLabel::_num_objetcs = 0;
Texture* NumberLabel::_textures(NULL);
float NumberLabel::_texratio = 1.0;

void NumberLabel::_maketextures()
{

    if(_num_objetcs==0) {
        cout << "make tex" << endl;

        _textures = new Texture[NUMBERLABEL_NUM_TEX];

        const char chars[] = NUMBERLABEL_CHARS;

        for( int i=0; i<NUMBERLABEL_NUM_TEX; ++i ) {
            stringstream s;
            s << chars[i];
            TextRenderer::I().Text2Texture( _textures[i], s.str());
        }

    }
    _num_objetcs++;

}
