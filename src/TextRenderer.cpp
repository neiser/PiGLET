#include "TextRenderer.h"
#include <iostream>
#include <sstream>
#include <stdint.h>  // for uint32_t
#include <iomanip>
#include "magick/MagickCore.h"
#include "GLTools.h"

using namespace std;

unsigned TextRenderer::count = 0;

TextRenderer::TextRenderer() : _buffer(NULL)
{
    if(count == 0)
        MagickWandGenesis();    // just once at the beginning
    count++;
    _mw = NewMagickWand();
    //SetTextOptions(_mw); // set some defaults
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
    //SetTextOptions(_mw);
    
    delete [] _buffer;
    _buffer = NULL;
}



void TextRenderer::Text2Texture(Texture& tex, const string &text )
{
    std::stringstream rendercmd;
    rendercmd << "label:" << text;
    SetTextOptions();    
    MagickReadImage(_mw, rendercmd.str().c_str());
    InitWidthHeightUV(); // do that before the image gets extended!
    MagickExtentImage(_mw,_w_pow2,_h_pow2,0,0); // here we used the init'ed values!
    CopyToBuffer(GL_LUMINANCE );
    
    BindTexture(tex, GL_LUMINANCE);
}

void TextRenderer::SetTextOptions()
{
    MagickSetSize(_mw,0,0);    
    MagickSetPointsize(_mw,48);
    MagickSetFont(_mw,"DejaVu-Sans-Mono-Book");
    MagickSetOption(_mw,"colorspace","GRAY");    // does this do anything?
    MagickSetOption(_mw,"fill","white");
    MagickSetOption(_mw,"background","rgba(0,0,0,0)");   // transparent background
    MagickSetOption(_mw,"encoding","unicode");   // does this do anything?
    MagickSetGravity(_mw,CenterGravity);
}

void TextRenderer::DrawCrossHair(const size_t &x, const size_t &y, const size_t &size)
{
    // this is necessary for the other options 
    // of dw to show an effect...
    MagickSetOption(_mw,"stroke","1"); 
    // how to use transparency? I don't know. The next command has no effect...
    //MagickSetImageAlphaChannel(_mw, ActivateAlphaChannel);
    DrawingWand* dw = NewDrawingWand();
    DrawSetStrokeAntialias(dw,MagickTrue);
    
    DrawBlackWhiteLine(dw,x-size/2,y,x+size/2,y);
    DrawBlackWhiteLine(dw,x,y-size/2,x,y+size/2);    
    
    MagickDrawImage(_mw,dw);
    
    DestroyDrawingWand(dw);
}

void TextRenderer::DrawRect(const size_t &x, const size_t &y, const size_t &size)
{
    // this is necessary for the other options 
    // of dw to show an effect...
    MagickSetOption(_mw,"stroke","1");
    MagickSetOption(_mw,"fill","none"); 
    // how to use transparency? I don't know. The next command has no effect...
    //MagickSetImageAlphaChannel(_mw, ActivateAlphaChannel);
    DrawingWand* dw = NewDrawingWand();
    DrawSetStrokeAntialias(dw,MagickTrue);
    
    // draw a thicker black rectangle
    PixelWand* cw_b = NewPixelWand();
    PixelSetColor(cw_b,"rgb(0,0,0)");    
    DrawSetStrokeColor(dw,cw_b);
    DrawSetStrokeWidth(dw,4);
    DrawRectangle(dw, x-size/2, y-size/2, x+size/2, y+size/2);
    DestroyPixelWand(cw_b);  
    
    // draw a thicker black rectangle
    PixelWand* cw_w = NewPixelWand();
    PixelSetColor(cw_w,"rgb(255,255,255)");    
    DrawSetStrokeColor(dw,cw_w);
    DrawSetStrokeWidth(dw,2);
    DrawRectangle(dw, x-size/2, y-size/2, x+size/2, y+size/2);
    DestroyPixelWand(cw_w);  
    
    MagickDrawImage(_mw,dw);
    
    DestroyDrawingWand(dw);
}

void TextRenderer::DrawBlackWhiteLine(DrawingWand *dw, 
                                      const size_t &x1, const size_t &y1, 
                                      const size_t &x2, const size_t &y2)
{
    // draw a thicker black line
    PixelWand* cw_b = NewPixelWand();
    PixelSetColor(cw_b,"rgb(0,0,0)");    
    DrawSetStrokeColor(dw,cw_b);
    DrawSetStrokeWidth(dw,5);
    DrawLine(dw, x1, y1, x2, y2);
    DestroyPixelWand(cw_b);
    
    // draw a thinner white line on top
    PixelWand* cw_w = NewPixelWand();
    PixelSetColor(cw_w,"rgb(255,255,255)");    
    DrawSetStrokeColor(dw,cw_w);
    DrawSetStrokeWidth(dw,2);
    DrawLine(dw, x1, y1, x2, y2);
    DestroyPixelWand(cw_w);
}



void TextRenderer::Mw2Texture(Texture& tex)
{
    BindTexture(tex, GL_RGBA);
}

bool TextRenderer::Image2Mw(const string &url, 
                            const size_t &crop_w, const size_t &crop_h, 
                            const size_t &crop_x, const size_t &crop_y,
                            const size_t &crosshair_x, const size_t &crosshair_y, const size_t &crosshair_size, 
                            const size_t &rect_x, const size_t &rect_y, const size_t &rect_size)
{
    
    if(!MagickReadImage(_mw, url.c_str()))
        return false;
        
    MagickCropImage(_mw, 
                    crop_w==0 ? MagickGetImageWidth(_mw) : crop_w,
                    crop_h==0 ? MagickGetImageHeight(_mw) : crop_h,
                    crop_x, crop_y);

    if(crosshair_size != 0)
        DrawCrossHair(crosshair_x, crosshair_y, crosshair_size);
    if(rect_size != 0)
        DrawRect(rect_x, rect_y, rect_size);
    
    
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
