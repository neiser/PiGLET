#include "GLApp.h"

#include "Structs.h"
#include "BlockBuffer.h"

class PiGLPlotApp: public GLApp {
protected:
    static const vec2_t square[4];
    PiGLPlot::BlockList graph;

public:
    PiGLPlotApp();

    virtual void Init();
    virtual void Draw();

};
