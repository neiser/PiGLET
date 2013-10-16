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

    void CopyToTexture( const GLuint texhandle, const int width, const int height );

public:

    static TextRenderer& I() {
        static TextRenderer instance;
        return instance;
    }

    void Text2TextureFixedSize(const GLuint texhandle, const std::string &text, const int w, const int h, float& texw, float& texh);
    void Text2Texture3(const GLuint texhandle, const std::string &text, float& texw, float& texh, float& aspect);

};


class TextLabel: public Widget {
private:

    std::string _text;      // the text to draw
    GLuint      _texture;   // texture handle
    Color       _color;     // color for the text
    Rectangle   _rext;      // Maximum size of the text
    Rectangle   _box;       // the actual drawing box, always smaller than the user defined text rectangle
    vec2_t _texcoords[4];      // texture coordinates

public:

    TextLabel( const Window* owner, const float x1, const float y1,const float x2, const float y2);

    virtual ~TextLabel();

    void Draw() const;

    void SetText( const std::string& text );
    const std::string& GetText() const { return _text; }
    void SetColor( const Color& c ) { _color = c; }
    Color GetColor() const { return _color; }
};




#endif
