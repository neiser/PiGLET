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

std::string NumberLabel::FormatNumberPrec(const float& v, const char prefix) {
    std::stringstream stream;
    unsigned short prec = _prec;
    int exp = log10(abs(v));  // should never be negative...
    prec = prec>exp ? prec-exp : 0;
    
    stream << setprecision(prec) << fixed;
    stream << v << prefix;
    return stream.str();
}

std::string NumberLabel::FormatNumberSI(const float& v) {
    if(v == 0)
        return "0 ";
        
    
    const std::string prefixes = "pnum kMGT"; // no SI prefix in the center
    
    // find some good guess for the SI prefix exponential
    // make sure it's inside the bounds
    int si_exp = (int) (log10(abs(v)) / 3 );    
    int center = prefixes.size()/2; // integer division
    if( si_exp < -center )
        si_exp = -center;
    else if( si_exp > center )
        si_exp = center;
        
    // find string with minimum length, large numbers are usually
    // nicer displayed with one exp less
    std::string str = FormatNumberPrec(v / pow10(si_exp*3), prefixes[si_exp+center]);
    if(si_exp==1) { // prefix = k
        std::string str_noprefix = FormatNumberPrec(v, ' ');
        if(str_noprefix.size()-1<str.size())
            str = str_noprefix;
    }
    return str;
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
    // remember the value, if the precision is changed...
    _value = v;
    // generate text string from number
    SetString(FormatNumberSI(v));
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
