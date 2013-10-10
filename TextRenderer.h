#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <string>
#include <wand/magick_wand.h>
#include "system.h"
#include "StopWatch.h"
#include "Structs.h"


class TextRenderer {
protected:
    MagickWand *mw;
    StopWatch _watch;

private:
    TextRenderer();
    virtual ~TextRenderer();
    TextRenderer(TextRenderer const& copy);            // Not Implemented
    TextRenderer& operator=(TextRenderer const& copy); // Not Implemented

public:

    static TextRenderer& I() {
        static TextRenderer instance;
        return instance;
    }

    void test( const std::string& text );

    void Text2Texture(const GLuint texhandle, const std::string& text );


};

class Rectangle {
private:
    float _width;
    float _height;
    vec2_t _center;
    vec2_t _vertices[4];

    void _update_vertices();

public:
    Rectangle( const float x1, const float y1, const float x2, const float y2 );
    Rectangle( const vec2_t& center, const float width, const float height);

    virtual ~Rectangle() {}

    float Width() const { return _width; }
    float Height() const { return _height; }

    const vec2_t& Center() const { return _center; }

    void SetCenter( const vec2_t& center );
    void SetWidth( const float width );
    void SetHeight( const float height );

    void Draw( GLenum mode );

};

class TextLabel: public Rectangle {
private:
    std::string _text;
    GLuint _texture;
    static const vec2_t _texcoords[4];

public:
    TextLabel( const float x1, const float y1, const float x2, const float y2);
    TextLabel(const vec2_t& center, const float width, const float height);

    virtual ~TextLabel() {}

    void Draw( GLenum mode=GL_TRIANGLE_FAN );

    void SetText( const std::string& text );
    const std::string& GetText() const { return _text; }
};


#endif
