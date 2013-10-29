#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <time.h>

class StopWatch {
private:
    double _start;
    double _stop;

public:
    StopWatch();
    virtual ~StopWatch() {}

    void Start() { 
        timespec t;
        clock_gettime(CLOCK_MONOTONIC, &t); 
        _start = t.tv_sec + t.tv_nsec*1e-9;
    }
    
    void Start(const double offset) {
        Start();
        _start += offset;
    }
    
    void Stop() { 
        timespec t;
        clock_gettime(CLOCK_MONOTONIC, &t);
        _stop = t.tv_sec + t.tv_nsec*1e-9;
    }
    double TimeElapsed();

};


#endif
