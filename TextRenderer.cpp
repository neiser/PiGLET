#include "TextRenderer.h"
#include <iostream>
#include <sstream>
#include <stdint.h>  // for uint32_t
#include <iomanip>
using namespace std;


TextRenderer::TextRenderer()
{
    MagickWandGenesis();    // just once somewhere
    mw = NewMagickWand();

    MagickSetSize(mw,256,64);
    MagickSetPointsize(mw,48);
    MagickSetFont(mw,"Sans");
    MagickSetOption(mw,"colorspace","GRAY");    // does this do anything?
    MagickSetOption(mw,"fill","white");
    MagickSetOption(mw,"background","rgba(0,0,0,0)");   // transparent background
    MagickSetOption(mw,"encoding","unicode");   // does this do anything?
    MagickSetGravity(mw,CenterGravity);
}

TextRenderer::~TextRenderer()
{
    if(mw)
        DestroyMagickWand(mw);
}

void TextRenderer::Text2Texture(const GLuint texhandle, const string &text, const int w, const int h) {
    MagickSetSize(mw,w,h);
    Text2Texture(texhandle,text);
}

void TextRenderer::Text2Texture(const GLuint texhandle, const string &text)
{
    _watch.Start();

    std::stringstream rendercmd;
    rendercmd << "label:" << text;

    MagickReadImage(mw, rendercmd.str().c_str());

    cout << "handle=" << texhandle << " Text=" << text << endl;

    unsigned char *Buffer = NULL;
    size_t width = MagickGetImageWidth(mw);
    size_t height = MagickGetImageHeight(mw);
    Buffer = new unsigned char[width * height];

    cout << "w="<<width << " h="<<height << endl;

    // Export the whole image
    MagickExportImagePixels(mw, 0, 0, width, height, "I", CharPixel, Buffer);

    glBindTexture(GL_TEXTURE_2D, texhandle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, Buffer);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    delete [] Buffer;

    _watch.Stop();

    cout << "Text generation took: " << _watch.TimeElapsed() << " s" << endl;

}

void TextRenderer::Text2Texture2(const GLuint texhandle, const string &text, const int w, const int h, float& texw, float& texh)
{
    _watch.Start();

    MagickSetSize(mw,0,0);

    std::stringstream rendercmd;
    rendercmd << "label:" << text;

    MagickReadImage(mw, rendercmd.str().c_str());

    size_t _w = MagickGetImageWidth(mw);
    size_t _h = MagickGetImageHeight(mw);
    MagickExtentImage(mw,w,h,0,0);

    size_t width = MagickGetImageWidth(mw);
    size_t height = MagickGetImageHeight(mw);

    cout << "Generated: " << _w << "x" << _h << " padded:" << width << "x" << height << endl;

    unsigned char *Buffer = NULL;

    Buffer = new unsigned char[width * height];

    cout << "w="<<width << " h="<<height << endl;

    // Export the whole image
    MagickExportImagePixels(mw, 0, 0, width, height, "I", CharPixel, Buffer);

    glBindTexture(GL_TEXTURE_2D, texhandle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, Buffer);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    delete [] Buffer;

    _watch.Stop();

    cout << "Text generation took: " << _watch.TimeElapsed() << " s" << endl;

    texw = ((float)_w) / w;
    texh = ((float)_h) / w;

}

uint32_t RoundPow2( uint32_t val ) {
    val--;
    val = (val >> 1) | val;
    val = (val >> 2) | val;
    val = (val >> 4) | val;
    val = (val >> 8) | val;
    val = (val >> 16) | val;
    val++; // Val is now the next highest power of 2.
    return val;
}

void TextRenderer::Text2Texture3(const GLuint texhandle, const string &text, float& texw, float& texh)
{
    _watch.Start();

    MagickSetSize(mw,0,0);

    std::stringstream rendercmd;
    rendercmd << "label:" << text;

    MagickReadImage(mw, rendercmd.str().c_str());

    const size_t _w = MagickGetImageWidth(mw);
    const size_t _h = MagickGetImageHeight(mw);

    const size_t nw = RoundPow2( _w );
    const size_t nh = RoundPow2( _h );

    MagickExtentImage(mw,nw,nh,0,0);

    const size_t width = MagickGetImageWidth(mw);
    const size_t height = MagickGetImageHeight(mw);

    cout << "Generated: " << _w << "x" << _h << " padded:" << width << "x" << height << "  calc was " << nw << "x" << nh << endl;

    unsigned char *Buffer = NULL;

    Buffer = new unsigned char[width * height];

    // Export the whole image
    MagickExportImagePixels(mw, 0, 0, width, height, "I", CharPixel, Buffer);

    glBindTexture(GL_TEXTURE_2D, texhandle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, Buffer);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    delete [] Buffer;

    _watch.Stop();

    cout << "Text generation took: " << _watch.TimeElapsed() << " s" << endl;

    texw = ((float)_w) / width;
    texh = ((float)_h) / height;

}

void Rectangle::_update_vertices()
{
    _vertices[0].x = _center.x - _width/2.0f;
    _vertices[0].y = _center.y - _height/2.0f;

    _vertices[1].x = _center.x - _width/2.0f;
    _vertices[1].y = _center.y + _height/2.0f;

    _vertices[2].x = _center.x + _width/2.0f;
    _vertices[2].y = _center.y + _height/2.0f;

    _vertices[3].x = _center.x + _width/2.0f;
    _vertices[3].y = _center.y - _height/2.0f;
}

Rectangle::Rectangle(const float x1, const float y1, const float x2, const float y2)
{
    _center.x = (x1 + x2) / 2.0f;
    _center.y = (y1 + y2) / 2.0f;
    _width    = x2 - x1;
    _height   = y2 - y1;

    _update_vertices();
}

Rectangle::Rectangle( const vec2_t& center, const float width, const float height): _center(center), _width(width), _height(height)
{
    _update_vertices();
}

void Rectangle::SetCenter(const vec2_t &center)
{
    _center = center;
    _update_vertices();
}

void Rectangle::SetWidth(const float width)
{
    _width = width;
    _update_vertices();
}

void Rectangle::SetHeight(const float height)
{
    _height = height;
    _update_vertices();
}

void Rectangle::Draw(GLenum mode)
{
    glVertexPointer( 2, GL_FLOAT, 0, _vertices);
    glDrawArrays( mode, 0, 4);
}


TextLabel::~TextLabel()
{
    glDeleteTextures(1, &_texture);
}

TextLabel::TextLabel(const float x1, const float y1, const float x2, const float y2):
    Rectangle(x1, y1, x2, y2), _color( dTextColor ),_box(*this)
{
    glGenTextures(1,&_texture);
}

void TextLabel::Draw(GLenum mode)
{

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    _color.Activate();

    glTexCoordPointer(2, GL_FLOAT, 0, _texcoords);

    glBindTexture(GL_TEXTURE_2D, _texture);
    Rectangle::Draw( GL_TRIANGLE_FAN );

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //glLineWidth(1);
   // _box.Draw( GL_LINE_LOOP );
   // Rectangle::Draw( GL_LINE_LOOP );

}

void TextLabel::SetText(const string &text)
{
    float w,h;
    TextRenderer::I().Text2Texture3(_texture, text, w,h);

    const float tex_ratio = w/h;

    if( tex_ratio >= 1.0 ) {
        _box= Rectangle( Center(), Width(), 1.0/tex_ratio * Width() );
    } else {
        _box= Rectangle( Center(), tex_ratio * Height(), Height() );
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

        r.SetWidth(maxw/maxh);
    }

}

NumberLabel::~NumberLabel()
{
    --_num_objetcs;

    if( _num_objetcs == 0) {
        glDeleteTextures(NUMBERLABEL_NUM_TEX, _textures);
    }
}

vec2_t NumberLabel::_texcoords[4];

void NumberLabel::Draw()
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);

    _color.Activate();

    glTexCoordPointer(2, GL_FLOAT, 0, _texcoords);

    glPushMatrix();
    float s = 1.0/(6*r.Width());
    glScalef(s,s,s);
    glTranslatef(.5*r.Width(),0,0);

    for( int p=0;p<_digtex.size();++p ) {
        glBindTexture(GL_TEXTURE_2D, _digtex[p]);
        r.Draw( GL_TRIANGLE_FAN );
        glTranslatef(-r.Width(),0,0);
    }
    glPopMatrix();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
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
Rectangle NumberLabel::r(0,0,1,1);
