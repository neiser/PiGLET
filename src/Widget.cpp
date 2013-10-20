#include "Widget.h"
#include "Window.h"

#include <cmath>
#include <iostream>

using namespace std;

float Widget::GetWindowAspect() const
{
    return _owner->XPixels() / _owner->YPixels();
}
