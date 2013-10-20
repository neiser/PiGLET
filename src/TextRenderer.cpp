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
    _mw_text = NewMagickWand();
    InitMw(_mw_text); // set some 
    
}

void TextRenderer::InitMw(MagickWand* mw)
{
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
    if(_mw_text)
        DestroyMagickWand(_mw_text);
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

void TextRenderer::CopyToTexture(MagickWand* mw, Texture& tex, 
                                 const int width, const int height, GLenum TextureMode ) {

    string exportMode = "I";
    unsigned char bytes = 1;

    switch (TextureMode) {
    case GL_LUMINANCE:
        exportMode = "I";
        bytes = 1;
        break;
    case GL_RGBA:
        exportMode = "RGBA";
        bytes = 4;
        break;
    case GL_RGB:
        exportMode = "RGB";
        bytes = 3;
        break;
    default:
        cout << "CopyTexture: Invalid Mode" << endl;
        break;
    }

    unsigned char *Buffer = NULL;

    Buffer = new unsigned char[width * height * bytes];

    // Export the whole image
    MagickExportImagePixels(mw, 0, 0, width, height, exportMode.c_str(), CharPixel, Buffer);
    ClearMagickWand(mw);
    InitMw(mw);
    glBindTexture(GL_TEXTURE_2D, tex.GetTexHandle());

    glTexImage2D(GL_TEXTURE_2D, 0, TextureMode, width, height, 0, TextureMode, GL_UNSIGNED_BYTE, Buffer);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    delete [] Buffer;
    
}



void TextRenderer::Text2Texture( Texture& tex, const string &text )
{
    MagickSetSize(_mw_text,0,0);

    std::stringstream rendercmd;
    rendercmd << "label:" << text;

    MagickReadImage(_mw_text, rendercmd.str().c_str());

    const size_t _w = MagickGetImageWidth(_mw_text);
    const size_t _h = MagickGetImageHeight(_mw_text);

    const size_t nw = RoundPow2( _w );
    const size_t nh = RoundPow2( _h );

    MagickExtentImage(_mw_text,nw,nh,0,0);

    const size_t width = MagickGetImageWidth(_mw_text);
    const size_t height = MagickGetImageHeight(_mw_text);

    CopyToTexture(_mw_text, tex, width, height, GL_LUMINANCE );


    float texw = ((float)_w) / width;
    float texh = ((float)_h) / height;

    tex.SetMaxUV( texw, texh );
    tex.SetAspect((float) _w / (float) _h);
}

void TextRenderer::Image2Texture(MagickWand* mw, Texture& tex)
{
    
    const size_t width = MagickGetImageWidth(mw);
    const size_t height = MagickGetImageHeight(mw);
    
    const size_t nw = RoundPow2( width );
    const size_t nh = RoundPow2( height );
    
    CopyToTexture(mw, tex, nw, nh, GL_RGBA);
    
    tex.SetMaxUV( (float) width / nw, (float) height / nh);
    tex.SetAspect( (float) width / (float) height );
    
}

