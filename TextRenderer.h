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

public:

    static TextRenderer& I() {
        static TextRenderer instance;
        return instance;
    }

    void Text2Texture(const GLuint texhandle, const std::string& text );
    void Text2Texture(const GLuint texhandle, const std::string& text, const int w, const int h);
    void Text2Texture2(const GLuint texhandle, const std::string &text, const int w, const int h, float& texw, float& texh);
    void Text2Texture3(const GLuint texhandle, const std::string &text, float& texw, float& texh, float& aspect);


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

class TextLabel: public Widget {
private:

    std::string _text;      // the text to draw
    GLuint      _texture;   // texture handle
    Color       _color;     // color for the text
    Rectangle   _rext;      // Maximum size of the text
    Rectangle   _box;       // the actual drawing box, always smaller than the user defined text rectangle
    vec2_t _texcoords[4];      // texture coordinates

public:

    TextLabel( Window* owner, const float x1, const float y1,const float x2, const float y2);

    virtual ~TextLabel();

    void Draw( GLenum mode=GL_TRIANGLE_FAN );

    void SetText( const std::string& text );
    const std::string& GetText() const { return _text; }
    void SetColor( const Color& c ) { _color = c; }
    Color GetColor() const { return _color; }
};


class NumberLabel: public Widget {
private:

#define NUMBERLABEL_NUM_TEX 14
#define NUMBERLABEL_CHARS "0123456789.-E?"

    static GLuint _textures[NUMBERLABEL_NUM_TEX];
    static vec2_t _texcoords[4];

    static unsigned int _num_objetcs;

    static Rectangle r;
    void _maketextures();
    std::vector<GLuint> _digtex;

    Color _color;
    unsigned char _prec;

public:
    NumberLabel( Window* owner ): Widget(owner), _color(dTextColor), _prec(2) {}
    virtual ~NumberLabel();

    void Draw();
    void Set( const float v );
    void Init() { _maketextures(); }

    void SetColor( const Color& c ) { _color = c; }
    Color GetColor() const { return _color; }

    /**
     * @brief Sets the decimal precision
     * @param prec number of digits
     * @note  Gets applied at the next call to Set()
     * @see GetPrec()
     */
    void SetPrec( const unsigned char prec ) { _prec = prec; }

    /**
     * @brief Get the decimal precision
     * @return number of digits
     * @see SetPrec()
     */
    unsigned char GetPrec() const { return _prec; }
};


#endif
