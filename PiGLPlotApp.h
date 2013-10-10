#include "GLApp.h"

#include "Structs.h"
#include "BlockBuffer.h"
#include "TextRenderer.h"

#include "WindowManager.h"
#include "Window.h"


class PiGLPlotApp: public GLApp {
protected:
    static const vec2_t square[4];
    static const vec2_t square_tex[4];
    PiGLPlot::BlockList graph;

    float phase;

    TextLabel text;

    int frame;

    NumberLabel num;

public:
    PiGLPlotApp();

    virtual void Init();
    virtual void Draw();

};

class testApp: public GLApp{
//private:

public:

    WindowManager windowman;

    testApp():windowman(){}

    virtual void Init();
    virtual void Draw();
};
