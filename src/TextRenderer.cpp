#include "TextRenderer.h"
#include <iostream>
#include <sstream>
#include <stdint.h>  // for uint32_t
#include <iomanip>
#include "GLTools.h"

using namespace std;

unsigned TextRenderer::count = 0;

TextRenderer::TextRenderer() : _buffer(NULL)
{
    if(count == 0)
        MagickWandGenesis();    // just once at the beginning
    count++;
    _mw = NewMagickWand();
    InitMw(_mw); // set some defaults
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
    DestroyMagickWand(_mw);
    count--;
    if(count==0)
        MagickWandTerminus();    
}

uint32_t TextRenderer::RoundPow2( uint32_t val ) {
    val--;
    val = (val >> 1) | val;
    val = (val >> 2) | val;
    val = (val >> 4) | val;
    val = (val >> 8) | val;
    val = (val >> 16) | val;
    val++; // Val is now the next highest power of 2.
    return val;
}

void TextRenderer::CopyToBuffer(GLenum textureMode) {
    
    string exportMode = "I";
    unsigned char bytes = 1;
    
    switch (textureMode) {
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
    
    _buffer = new unsigned char[_w_pow2 * _h_pow2 * bytes];
    
    // Export the whole image
    MagickExportImagePixels(_mw, 0, 0, _w_pow2, _h_pow2, 
                            exportMode.c_str(), CharPixel, _buffer);
    
    
}

void TextRenderer::BindTexture(Texture &tex, GLenum textureMode)
{
    glBindTexture(GL_TEXTURE_2D, tex.GetTexHandle());
    
    glTexImage2D(GL_TEXTURE_2D, 0, textureMode, _w_pow2, _h_pow2, 
                 0, textureMode, GL_UNSIGNED_BYTE, _buffer);
    
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    tex.SetMaxUV( _u, _v);
    tex.SetAspect(_aspect_orig);
    
    // clearing and init prevents memory eating
    // clear it here finally since properties of the image are still used
    ClearMagickWand(_mw);
    InitMw(_mw);
    
    delete [] _buffer;
    _buffer = NULL;
}



void TextRenderer::Text2Texture(Texture& tex, const string &text )
{
    std::stringstream rendercmd;
    rendercmd << "label:" << text;
    MagickSetSize(_mw,0,0);
    MagickReadImage(_mw, rendercmd.str().c_str());
    InitWidthHeightUV(); // do that before the image gets extended!
    MagickExtentImage(_mw,_w_pow2,_h_pow2,0,0); // here we used the init'ed values!
    
    CopyToBuffer(GL_LUMINANCE );
    
    BindTexture(tex, GL_LUMINANCE);
}

void TextRenderer::Mw2Texture(Texture& tex)
{
    BindTexture(tex, GL_RGBA);
}

bool TextRenderer::Image2Mw(const string &url, const size_t &w, const size_t &h, const size_t &x, const size_t &y)
{
    
    if(!MagickReadImage(_mw, url.c_str()))
        return false;
        
    MagickCropImage(_mw, 
                    w==0 ? MagickGetImageWidth(_mw) : w,
                    h==0 ? MagickGetImageHeight(_mw) : h,
                    x, y);
    
    InitWidthHeightUV();    
    CopyToBuffer(GL_RGBA);
    return true;
}

void TextRenderer::InitWidthHeightUV()
{
    const size_t width = MagickGetImageWidth(_mw);
    const size_t height = MagickGetImageHeight(_mw);
    
    _aspect_orig = (float) width / (float) height;
    
    _w_pow2 = RoundPow2(width);
    _h_pow2 = RoundPow2(height);
    
    _u = (float) width / _w_pow2;
    _v = (float) height / _h_pow2;
}
