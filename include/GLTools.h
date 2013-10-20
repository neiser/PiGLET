#ifndef GLTOOLS_H
#define GLTOOLS_H
#include "system.h"
#include "Structs.h"

class Vector2 {
private:
    vec2_t _v;

public:

    Vector2( const vec2_t& v ): _v(v) {}
    Vector2( const float& x=0.0f, const float& y=0.0f ) {
        _v.x = x;
        _v.y = y;
    }

    const vec2_t& Stuct() const { return _v; }

    const float& X() const { return _v.x; }
    const float& Y() const { return _v.y; }

    float& X() { return _v.x; }
    float& Y() { return _v.y; }

    void operator+= ( const Vector2& v ) { _v.x = v.X(); _v.y += v.Y(); }

};

class Color {
private:
    GLfloat _color[4];

public:
    Color( const float r=0, const float g=0, const float b=0, const float a=1.0f) {
        _color[0] = r;
        _color[1] = g;
        _color[2] = b;
        _color[3] = a;
    }

    inline float& Red()   { return _color[0]; }
    inline float& Green() { return _color[1]; }
    inline float& Blue()  { return _color[2]; }
    inline float& Alpha() { return _color[3]; }

    inline const float& Red()   const { return _color[0]; }
    inline const float& Green() const { return _color[1]; }
    inline const float& Blue()  const { return _color[2]; }
    inline const float& Alpha() const { return _color[3]; }

    inline void Activate() const { glColor4fv( _color ); }

};

#define kRed    Color(1.0f, 0.0f, 0.0f)
#define kGreen  Color(0.0f, 1.0f, 0.0f)
#define kBlue   Color(0.0f, 0.0f, 1.0f)
#define kWhite  Color(1.0f, 1.0f, 1.0f)
#define kBlack  Color(0.0f, 0.0f, 0.0f)
#define kPink   Color(1.0f, 0.0f, 1.0f)
#define kYellow Color(1.0f, 1.0f, 0.0f)
#define kCyan   Color(0.0f, 1.0f, 1.0f)
#define kOrange Color(1.0f, 0.5f, 0.0f)
#define kDarkGray   Color(0.2f, 0.2f, 0.2f)
#define kGray   Color(0.4f, 0.4f, 0.4f)

#define dTextColor  kWhite
#define dBackColor  kBlack

#define dPlotBackground  kDarkGray
#define dPlotBorderColor  kGray
#define dPlotColor kBlue
#define dPlotTicks dPlotBorderColor
#define dPlotTickLabels dTextColor

#define dWindowBorderColor  kGray
#define dWindowBackground  dBackColor

#define dMinorAlarm kOrange
#define dMajorAlarm kRed
#define dInvalidAlarm kPink


class Rectangle {
private:
    float _width;
    float _height;
    Vector2 _center;
    vec2_t _vertices[4];

    void _update_vertices();

public:
    Rectangle( const float x1, const float y1, const float x2, const float y2 );
    Rectangle( const Vector2& center, const float width, const float height);

    virtual ~Rectangle() {}

    float Width() const { return _width; }
    float Height() const { return _height; }
    const Vector2& Center() const { return _center; }

    void SetCenter( const Vector2& center );
    void SetWidth( const float width );
    void SetHeight( const float height );

    void Draw( GLenum mode ) const;

    static const Rectangle unit;

};

class UnitBorderBox {
public:
    Color fillcolor;
    Color bordercolor;
    float borderwidth;

    UnitBorderBox( const Color& fill=dBackColor, const Color& border=dTextColor, const float& borderw=2.0f ):
        fillcolor(fill),
        bordercolor(border),
        borderwidth(borderw) {}

    void Draw() const {

        fillcolor.Activate();
        Rectangle::unit.Draw(GL_TRIANGLE_FAN);

        bordercolor.Activate();
        glLineWidth(borderwidth);
        Rectangle::unit.Draw(GL_LINE_LOOP);
    }

};

class Texture {
private:
    GLuint _tex;
    float  _aspect;

    vec2_t _texcoords[4];

public:
    Texture(): _tex(0), _aspect(0.0f) {
        glGenTextures(1, &_tex);
        SetMaxUV(0,0);
    }

    virtual ~Texture() {
        glDeleteTextures(1, &_tex);
    }

    void Activate() const {
        glTexCoordPointer(2, GL_FLOAT, 0, _texcoords);
        glBindTexture(GL_TEXTURE_2D, _tex);
    }

    const vec2_t* TextureCoords() const { return _texcoords; }
    float GetAspectRatio() const { return _aspect; }
    float GetMaxU() const { return _texcoords[2].x; }
    float GetMaxV() const { return _texcoords[1].y; }
    GLuint GetTexHandle() const { return _tex; }

    void SetMaxUV( const float maxu, const float maxv );
    void SetAspect( const float aspect ) { _aspect = aspect; }

};



#endif
