#ifndef WATCH_H
#define WATCH_H

#include <chrono>
#include <cstdint>

class Watch
{
    public:
        Watch();
        void start();
        void stop();
        void reset();
        
        // Retourne le temps écoulé en millisecondes
        uint64_t get_elapsed_time() const;
        
        // Utile pour initialiser un PRNG
        static uint64_t get_random_seed();

    private:
        using Clock = std::chrono::steady_clock;
        using TimePoint = std::chrono::time_point<Clock>;

        TimePoint _startTime;
        TimePoint _stopTime;
        bool _isRunning{false};
};

#endif