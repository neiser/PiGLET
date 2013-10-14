#include "GLApp.h"

#include "Structs.h"
#include "BlockBuffer.h"
#include "TextRenderer.h"

#include "WindowManager.h"
#include "Window.h"


class testApp: public GLApp{
//private:

public:

    WindowManager windowman;

    testApp():windowman(){}

    virtual void Init();
    virtual void Draw();
};
