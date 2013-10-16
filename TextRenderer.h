#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <string>
#include <wand/magick_wand.h>
#include "system.h"
#include "StopWatch.h"
#include "Structs.h"
#include "GLTools.h"
#include <vector>
#include "Widget.h"

class TextRenderer {
protected:
    MagickWand *mw;
    StopWatch _watch;

private:
    TextRenderer();
    virtual ~TextRenderer();
    TextRenderer(TextRenderer const& copy);            // Not Implemented
    TextRenderer& operator=(TextRenderer const& copy); // Not Implemented

    void CopyToTexture(const GLuint texhandle, const int width, const int height , GLenum TextureMode);

public:

    static TextRenderer& I() {
        static TextRenderer instance;
        return instance;
    }

    void Text2TextureFixedSize(const GLuint texhandle, const std::string &text, const int w, const int h, float& texw, float& texh);
    void Text2Texture3(const GLuint texhandle, const std::string &text, float& texw, float& texh, float& aspect);

    void LoadImage(const GLuint texhandle, const std::string url, float& texw, float& texh, float& aspect);

};


#endif
