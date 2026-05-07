#ifndef WATCH_H
#define WATCH_H

#include "platform.h"

class Watch
{
    public:
        Watch(void);
        void start();
        void stop();
        void reset();
        uint64_t get_elapsed_time();
        uint64_t get_random_seed();
    private:
        sys_time_t _startTime;
        sys_time_t _stopTime;
        bool _isRunning;
};

#endif
