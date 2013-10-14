#ifndef GLTOOLS_H
#define GLTOOLS_H
#include "system.h"

class Color {
private:
    GLfloat _color[4];

public:
    Color( const float r=0, const float g=0, float b=0, float a=1.0f) {
        _color[0] = r;
        _color[1] = g;
        _color[2] = b;
        _color[3] = a;
    }

    inline float& Red()   { return _color[0]; }
    inline float& Green() { return _color[1]; }
    inline float& Blue()  { return _color[2]; }
    inline float& Alpha() { return _color[3]; }

    inline void Activate() { glColor4fv( _color ); }

};

#define kRed    Color(1.0f, 0.0f, 0.0f)
#define kGreen  Color(0.0f, 1.0f, 0.0f)
#define kBlue   Color(0.0f, 0.0f, 1.0f)
#define kWhite  Color(1.0f, 1.0f, 1.0f)
#define kBlack  Color(0.0f, 0.0f, 0.0f)
#define kPink   Color(1.0f, 0.0f, 1.0f)
#define kYellow Color(1.0f, 1.0f, 0.0f)
#define kCyan   Color(0.0f, 1.0f, 1.0f)

#define dTextColor  kWhite
#define dBackColor  kBlack


#endif
