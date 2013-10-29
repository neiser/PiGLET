#include "StopWatch.h"

StopWatch::StopWatch()
{
   _stop = _start = 0;
}

double StopWatch::TimeElapsed()
{
    return _stop - _start;
}
