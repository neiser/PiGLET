#ifndef TEXTLABEL_H
#define TEXTLABEL_H

#include <string>

#include "Structs.h"
#include "GLTools.h"
#include "Widget.h"

class TextLabel: public Widget {
private:

    std::string _text;      // the text to draw
    Texture      _texture;   // texture handle
    Color       _color;     // color for the text
    Rectangle   _rext;      // Maximum size of the text
    Rectangle   _box;       // the actual drawing box, always smaller than the user defined text rectangle
    
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
