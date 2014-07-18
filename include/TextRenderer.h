#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <string>
#include <wand/magick_wand.h>
#include "arch.h"
#include "GLTools.h"

class TextRenderer {
  
private:
    static unsigned count;
    unsigned char *_buffer;
    
    MagickWand *_mw;
    
    // forbid copying
    TextRenderer(TextRenderer const& copy);            // Not Implemented
    TextRenderer& operator=(TextRenderer const& copy); // Not Implemented

    size_t _w_pow2, _h_pow2;
    float _u, _v, _aspect_orig;
    void InitWidthHeightUV();
    uint32_t RoundPow2( uint32_t val );
        
    void CopyToBuffer(GLenum TextureMode);
    void BindTexture(Texture& tex, 
                     GLenum TextureMode);
    
    void SetTextOptions();
    void DrawCrossHair(const size_t& x, const size_t& y, const size_t &size);
    void DrawRect(const size_t& x, const size_t& y, const size_t &size);
    static void DrawBlackWhiteLine(DrawingWand* dw,
                                   const size_t& x1, const size_t& y1,
                                   const size_t& x2, const size_t& y2); 
    static void MyDrawLine(DrawingWand* dw, const std::string& color, 
                                const size_t &stroke, 
                                const size_t &x1, const size_t &y1, 
                                const size_t &x2, const size_t &y2);
    static void MyDrawRect(DrawingWand* dw, const std::string& color, 
                           const size_t &stroke, 
                           const size_t &x, const size_t &y, 
                           const size_t &size);
    
public:

    TextRenderer();
    virtual ~TextRenderer();
        
    void Text2Texture( Texture& tex, const std::string &text );
    void Mw2Texture(Texture &tex);
    bool Image2Mw(const std::string& url, 
                  const size_t& crop_w = 0, const size_t& crop_h = 0, 
                  const size_t& crop_x = 0, const size_t& crop_y = 0,
                  const size_t& crosshair_x = 0, const size_t& crosshair_y = 0, 
                  const size_t& crosshair_size = 0,
                  const size_t& rect_x = 0, const size_t& rect_y = 0, 
                  const size_t& rect_size = 0);
};


#endif
