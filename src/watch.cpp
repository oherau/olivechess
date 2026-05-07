#include "watch.h"

Watch::Watch()
{
    reset();
}

void Watch::start()
{
    _startTime = Clock::now();
    _isRunning = true;
}

void Watch::stop()
{
    _stopTime = Clock::now();
    _isRunning = false;
}

void Watch::reset()
{
    _startTime = _stopTime = Clock::now();
    _isRunning = false;
}

// Récupère le temps écoulé en millisecondes
uint64_t Watch::get_elapsed_time() const
{
    TimePoint end = _isRunning ? Clock::now() : _stopTime;
    
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - _startTime).count();
}

// Retourne le temps actuel sous forme de graine (seed)
uint64_t Watch::get_random_seed()
{
    return static_cast<uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
    );
}