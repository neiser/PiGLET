#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <string>
#include <wand/magick_wand.h>
#include "arch.h"
#include "GLTools.h"

class TextRenderer {
  
private:
    static unsigned count;
    
    MagickWand *_mw;
    
    // forbid copying
    TextRenderer(TextRenderer const& copy);            // Not Implemented
    TextRenderer& operator=(TextRenderer const& copy); // Not Implemented

    uint32_t RoundPow2( uint32_t val );
    void CopyToTexture(Texture& tex, 
                       const int width, const int height, 
                       GLenum TextureMode);
    void InitMw(MagickWand* mw);
    
public:

    TextRenderer();
    virtual ~TextRenderer();
        
    void Text2Texture( Texture& tex, const std::string &text );
    void Mw2Texture(Texture &tex);
    bool Image2Mw(const std::string& url);
};


#endif
