#include "StopWatch.h"

StopWatch::StopWatch()
{
    _start.tv_sec =0;
    _start.tv_nsec=0;
    _stop = _start;
}

double StopWatch::TimeElapsed()
{
    return time_difference( _start, _stop );
}
