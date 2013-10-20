#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <time.h>

class StopWatch {
public:
    //typedef struct timespec timespec;

    /**
     * @brief Calculate the differnce in seconds of two timespec values
     * @param start The first (earlier) time point
     * @param stop The later time point
     * @return difference in seconds
     */
    float time_difference( const timespec& start, const timespec& stop ) {
        float time = stop.tv_sec - start.tv_sec + (stop.tv_nsec - start.tv_nsec) * 1E-9;
        return time;
    }

protected:
    timespec _start;
    timespec _stop;

public:
    StopWatch();
    virtual ~StopWatch() {}

    void Start() { clock_gettime(CLOCK_MONOTONIC, &_start); }
    void Stop() { clock_gettime(CLOCK_MONOTONIC, &_stop);}
    float TimeElapsed();

};


#endif
