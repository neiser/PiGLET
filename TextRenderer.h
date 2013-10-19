#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <string>
#include <wand/magick_wand.h>
#include "system.h"
#include "GLTools.h"

class TextRenderer {
protected:
    MagickWand *mw;

private:
    TextRenderer();
    virtual ~TextRenderer();
    TextRenderer(TextRenderer const& copy);            // Not Implemented
    TextRenderer& operator=(TextRenderer const& copy); // Not Implemented

    static void CopyToTexture(MagickWand* mw, Texture& tex, const int width, const int height , GLenum TextureMode);

public:

    static TextRenderer& I() {
        static TextRenderer instance;
        return instance;
    }

    void Text2Texture( Texture& tex, const std::string &text );
    static void Image2Texture(MagickWand* mw, Texture &tex);

};


#endif
