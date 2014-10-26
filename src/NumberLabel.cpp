#include "NumberLabel.h"
#include <sstream>
#include <string>
#include <iomanip>
#include "TextRenderer.h"
#include <algorithm>
#include <cmath>

#include <iostream>

using namespace std;

static float pow10( const int exp ) {
    float v=1;
    if( exp >= 0 ) {
        for( int i=0; i < exp; ++i) {
            v *= 10.0;
        }
    }
    else {
        for( int i=0; i < -exp; ++i) {
            v /= 10.0;
        }
    }

    return v;
}

#define NUM_PREFIX 9
static char prefixes[NUM_PREFIX+1] = "pnum kMGT";

static std::ostream& SIPrefix( std::ostream& stream, const float& v ) {

   if( v != 0.0f ) {

        int exp = (int) (log10(abs(v)) / 3 );

        if( exp < -NUM_PREFIX/2 )
            exp = -NUM_PREFIX/2;
        else if( exp > NUM_PREFIX/2 )
            exp = NUM_PREFIX/2;

        stream << v / pow10(exp*3) << prefixes[ exp + (NUM_PREFIX/2) ];

   } else {
       stream << v << " ";
   }

    return stream;
}


NumberLabel::NumberLabel(const Window *owner ):
    Widget(owner),
    _color(dTextColor),
    _prec(2),
    _value(nanf("")),
    _digits(7),
    _align_right(true),
    _draw_box(true)
{
    _maketextures();
}

NumberLabel::~NumberLabel()
{
    --_num_objects;

    if( _num_objects == 0) {
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

        for(size_t p=0; p<_digtex.size(); ++p ) {

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

void NumberLabel::SetString(const std::string &str)
{
    _digtex.resize(str.size());

    for(size_t p=0; p< str.size(); ++p) {

        const char& c = str[p];
        unsigned char texnum;

        texnum = c - 32;

        _digtex[str.size() - p-1] = texnum;

    }
}

void NumberLabel::SetPrec(const unsigned short prec)
{
     _prec = prec; 
     if(!isnan(_value))
         SetNumber(_value);
}

void NumberLabel::SetNumber(const float v)
{
    _value = v;
    // generate text string from number
    stringstream stream;
    stream << fixed << setprecision(_prec);
    SIPrefix(stream,v);

    string s = stream.str();
    SetString(s);

}

void NumberLabel::SetTime( float s) {
    const char m[6] = "smhd";
    int level =0;

    if( abs(s) >= 60.0f ) {
        s /= 60.0;
        level++;
        if( abs(s) >= 60.0f ) {
            s /= 60.0;
            level++;
            if( abs(s) >= 24.0f ) {
                s /= 24.0f;
                level++;
            }
        }
    }

    stringstream text;
    text << fixed << setprecision(2) << s << " " << m[level];
    SetString(text.str());

}



unsigned int NumberLabel::_num_objects = 0;
Texture* NumberLabel::_textures(NULL);
float NumberLabel::_texratio = 1.0;

void NumberLabel::_maketextures()
{

    if(_num_objects==0) {
        cout << "make tex" << endl;

        _textures = new Texture[NUMBERLABEL_NUM_TEX];

        const char chars[] = NUMBERLABEL_CHARS;

        TextRenderer render;
        for( int i=0; i<NUMBERLABEL_NUM_TEX; ++i ) {
            stringstream s;
            s << chars[i];
            render.Text2Texture( _textures[i], s.str());
        }

    }
    _num_objects++;

}
