#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <time.h>

class StopWatch {
private:
    double _start;
    double _stop;

    double GetTime() {
        timespec t;
        clock_gettime(CLOCK_MONOTONIC, &t);
        return t.tv_sec + t.tv_nsec*1e-9;
    }
public:
    StopWatch();
    virtual ~StopWatch() {}
        
    void Start() { 
        _start = GetTime();
    }
    
    void Start(const double offset) {
        Start();
        _start += offset;
    }
    
    void Stop() { 
        _stop = GetTime();
    }
    double TimeElapsed();

};


#endif
