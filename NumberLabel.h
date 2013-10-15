#ifndef NUMBERLABEL_H
#define NUMBERLABEL_H

#include "system.h"
#include "GLTools.h"
#include "Widget.h"

class NumberLabel: public Widget {
private:

#define NUMBERLABEL_NUM_TEX 14
#define NUMBERLABEL_CHARS "0123456789.-E?"

    static GLuint _textures[NUMBERLABEL_NUM_TEX];
    static vec2_t _texcoords[4];

    static unsigned int _num_objetcs;

    static Rectangle r;

    static void _maketextures();
    std::vector<GLuint> _digtex;

    Color _color;
    unsigned char _prec;
    unsigned char _digits;

public:
    NumberLabel( Window* owner, const Vector2& pos );
    virtual ~NumberLabel();

    void Draw();
    void Set( const float v );
    void Init() {  }

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
