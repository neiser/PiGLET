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
    
    void InitMw(MagickWand* mw);
     
    
public:

    TextRenderer();
    virtual ~TextRenderer();
        
    void Text2Texture( Texture& tex, const std::string &text );
    void Mw2Texture(Texture &tex);
    bool Image2Mw(const std::string& url, 
                  const size_t& w = 0, const size_t& h = 0, 
                  const size_t& x = 0, const size_t& y = 0);
};


#endif
