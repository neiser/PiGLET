#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <string>
#include <wand/magick_wand.h>
#include "arch.h"
#include "GLTools.h"

class TextRenderer {
  
private:
    MagickWand *_mw_text;
    
    TextRenderer();
    virtual ~TextRenderer();
    TextRenderer(TextRenderer const& copy);            // Not Implemented
    TextRenderer& operator=(TextRenderer const& copy); // Not Implemented

    static void CopyToTexture(MagickWand* mw, Texture& tex, const int width, const int height , GLenum TextureMode);
    static void InitMw(MagickWand* mw);
public:

    static TextRenderer& I() {
        static TextRenderer instance;
        return instance;
    }

    void Text2Texture( Texture& tex, const std::string &text );
    static void Image2Texture(MagickWand* _mw_text, Texture &tex);

};


#endif
