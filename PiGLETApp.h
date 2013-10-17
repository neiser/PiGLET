#include "GLApp.h"

#include "Structs.h"
#include "BlockBuffer.h"
#include "TextRenderer.h"

#include "WindowManager.h"
#include "Window.h"


class PiGLETApp: public GLApp{
//private:

public:

    WindowManager windowman;

    PiGLETApp():windowman(){}

    virtual void Init();
    virtual void Draw();
};
