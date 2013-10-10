#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <string>
#include <wand/magick_wand.h>
#include "system.h"
#include "StopWatch.h"


class TextRenderer {
protected:
    MagickWand *mw;
    StopWatch _watch;


public:
    TextRenderer();
    virtual ~TextRenderer();

    void test( const std::string& text );

    void Text2Texture(const GLuint texhandle, const std::string& text );


};


#endif
