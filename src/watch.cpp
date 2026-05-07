#include <stdlib.h>
#include "platform.h"
#include "watch.h"

Watch::Watch(void)
{
    reset();
}

void Watch::start()
{
    system_time(&_startTime);
    _isRunning = true;
}

void Watch::stop()
{
    system_time(&_stopTime);
    _isRunning = false;
}

void Watch::reset()
{
    _startTime = _stopTime;
    _isRunning = false;
}

// Get elapsed time in millisec
uint64_t Watch::get_elapsed_time()
{
    if(_isRunning)
    {
        sys_time_t now;
        system_time(&now);
        return time_to_msec(now)-time_to_msec(_startTime);
    }
    return time_to_msec(_stopTime)-time_to_msec(_startTime);
}

uint64_t Watch::get_random_seed()
{
    sys_time_t now;
    system_time(&now);
    return time_to_msec(now);
}


