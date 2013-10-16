#ifndef NUMBERLABEL_H
#define NUMBERLABEL_H

#include "system.h"
#include "GLTools.h"
#include "Widget.h"

class NumberLabel: public Widget {
private:

#define NUMBERLABEL_NUM_TEX 14
#define NUMBERLABEL_CHARS "0123456789.-E?"

    // array of texture handles
    static Texture* _textures;
    static vec2_t _texcoords[4];
    static unsigned int _num_objetcs;   // Number of instances alive

    /**
     * @brief Generate textures for all characters we can render
     */
    static void _maketextures();

    // aspect ratio of the textures
    static float _texratio;

    std::vector<GLuint> _digtex;

    Color _color;
    unsigned char _prec;
    unsigned char _digits;
    bool _align_right;
    bool _draw_box;

    UnitBorderBox Box;

public:
    NumberLabel( const Window* owner );
    virtual ~NumberLabel();

    void Draw() const;
    void Set( const float v );

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

    /**
     * @brief Sets the number of digits to aling text to
     * @param digits number of digits
     * @note  Gets applied at the next call to Set()
     * @see GetDigits()
     */
    void SetDigits( const unsigned char digits ) { _digits = digits; }

    /**
     * @brief Get the number of digits to align text to
     * @return number of digits
     * @see SetDigits();
     */
    unsigned char GetDigits() const { return _digits; }


    bool GetAlignRight() const { return _align_right; }
    void SetAlignRight(bool align_right) { _align_right = align_right; }

    void SetDrawBox( const bool box ) { _draw_box = box; }
    bool GetDrawBox() const { return _draw_box; }

};

#endif
