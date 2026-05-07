#if !defined(THREAD_H_INCLUDED)
#define THREAD_H_INCLUDED

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>
#include "engine.h"
#include "platform.h"
#include "position.h"

class Thread {
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

public:
    explicit Thread(void (Thread::*fn)());
    ~Thread();

    void wake_up();
    void idle_loop();
    void brain_main_thread_loop();
    void clock_thread_loop();

    int idx;
    std::atomic<int> maxPly{0};
    
    std::mutex sleepLock;
    std::condition_variable sleepCond;
    std::jthread nativeThread;
    
    std::atomic<bool> do_sleep{false};
    std::atomic<bool> do_exit{false};
};

class ThreadsManager {
public:
    void init();
    ~ThreadsManager();

    void DisplayStatus();

    // Retourne une référence à l'objet contenu dans le unique_ptr
    Thread& operator[](size_t id) { 
        return *threads[id]; 
    }

    size_t size() const { 
        return threads.size(); 
    }

    // CORRECTION : Utilisation de .get() pour retourner le pointeur brut
    Thread* main_thread() { 
        return threads.empty() ? nullptr : threads[0].get(); 
    }

    void wake_up() const;
    void sleep() const;
    void read_uci_options();
    void set_timer(int msec);
    void wait_for_search_finished();

private:
    std::vector<std::unique_ptr<Thread>> threads;
    std::unique_ptr<Thread> timer_thread;
    std::unique_ptr<Thread> engine_thread;
};

extern ThreadsManager Threads;

#endif