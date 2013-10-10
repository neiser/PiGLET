#include "GLApp.h"

#include "Structs.h"
#include "BlockBuffer.h"
#include "TextRenderer.h"

class PiGLPlotApp: public GLApp {
protected:
    static const vec2_t square[4];
    static const vec2_t square_tex[4];
    PiGLPlot::BlockList graph;

    float phase;

    TextRenderer tr;

    GLuint tex;

public:
    PiGLPlotApp();

    virtual void Init();
    virtual void Draw();

};
