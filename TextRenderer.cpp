#include "TextRenderer.h"
#include <iostream>
#include <sstream>
#include <stdint.h>  // for uint32_t
#include <iomanip>
#include "GLTools.h"

using namespace std;


TextRenderer::TextRenderer()
{
    MagickWandGenesis();    // just once somewhere
    mw = NewMagickWand();

    MagickSetSize(mw,256,64);
    MagickSetPointsize(mw,48);
    MagickSetFont(mw,"DejaVu-Sans-Mono-Book");
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

void TextRenderer::Text2TextureFixedSize(const GLuint texhandle, const string &text, const int w, const int h, float& texw, float& texh)
{
   // _watch.Start();

    MagickSetSize(mw,0,0);

    std::stringstream rendercmd;
    rendercmd << "label:" << text;

    MagickReadImage(mw, rendercmd.str().c_str());

    size_t _w = MagickGetImageWidth(mw);
    size_t _h = MagickGetImageHeight(mw);
    MagickExtentImage(mw,w,h,0,0);

    size_t width = MagickGetImageWidth(mw);
    size_t height = MagickGetImageHeight(mw);

    CopyToTexture( texhandle, width, height);

    //_watch.Stop();

    //cout << "Text generation took: " << _watch.TimeElapsed() << " s" << endl;

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

void TextRenderer::CopyToTexture( const GLuint texhandle, const int width, const int height ) {

    unsigned char *Buffer = NULL;

    Buffer = new unsigned char[width * height];

    // Export the whole image
    MagickExportImagePixels(mw, 0, 0, width, height, "I", CharPixel, Buffer);

    glBindTexture(GL_TEXTURE_2D, texhandle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, Buffer);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    delete [] Buffer;

}

void TextRenderer::Text2Texture3(const GLuint texhandle, const string &text, float& texw, float& texh, float& aspect)
{
    //_watch.Start();

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

    CopyToTexture( texhandle, width, height );

    //_watch.Stop();

  //  cout << "Text generation took: " << _watch.TimeElapsed() << " s" << endl;

    texw = ((float)_w) / width;
    texh = ((float)_h) / height;
    aspect = (float) _w / (float) _h;

}



TextLabel::~TextLabel()
{
    glDeleteTextures(1, &_texture);
}

TextLabel::TextLabel( Window* owner, const float x1, const float y1, const float x2, const float y2):
    Widget(owner), _rext(x1, y1, x2, y2), _color( dTextColor ),_box(_rext)
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

    _box.Draw( GL_TRIANGLE_FAN );

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}

void TextLabel::SetText(const string &text)
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


