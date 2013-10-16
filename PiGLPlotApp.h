#include "GLApp.h"

#include "Structs.h"
#include "BlockBuffer.h"
#include "TextRenderer.h"

#include "WindowManager.h"
#include "Window.h"


class PiGLPlotApp: public GLApp{
//private:

public:

    WindowManager windowman;

    PiGLPlotApp():windowman(){}

    virtual void Init();
    virtual void Draw();
};
