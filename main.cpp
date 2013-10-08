#include <iostream>

#include "Interval.h"
#include "RingBuffer.h"
#include "BlockBuffer.h"

using namespace std;
using namespace PiGLPlot;

int main()
{
    BlockList b(12);

    b.Draw();

    vec2_t a;

    for (int i =0; i<40; ++i ) {
        a.x = i;
        b.Add(a);
        b.Draw();
    }



    return 0;
}

