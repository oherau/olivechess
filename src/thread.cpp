#include "thread.h"
#include "notation.h"
#include "platform.h"
#include "engine.h"
#include "logger.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <chrono>
#include <cstring>

using namespace std;

// Constantes de temps converties pour std::chrono
constexpr auto WAITFORSTART_ENGINE_CLOCK = std::chrono::milliseconds(100);
constexpr auto MONITOR_ENGINE_CLOCK = std::chrono::milliseconds(1000);

ThreadsManager Threads; // Objet global

// --- Thread ---

Thread::Thread(void (Thread::*fn)()) {
    idx = static_cast<int>(Threads.size());
    do_sleep = (fn != &Thread::brain_main_thread_loop);
    do_exit = false;

    // Utilisation d'une lambda C++20 pour lancer le thread membre
    nativeThread = std::jthread([this, fn](std::stop_token st) {
        (this->*fn)();
    });
}

Thread::~Thread() {
    do_exit = true;
    brainStatus = QUITTING;
    wake_up();
    // std::jthread gère automatiquement le join() à la destruction
}

void Thread::wake_up() {
    {
        std::lock_guard<std::mutex> lk(sleepLock);
        do_sleep = false;
    }
    sleepCond.notify_one();
}

void Thread::clock_thread_loop() {
    while (brainStatus != QUITTING) {
        while (brainStatus != THINKING && brainStatus != QUITTING) {
            std::this_thread::sleep_for(WAITFORSTART_ENGINE_CLOCK);
        }
        
        if (brainStatus == QUITTING) return;

        if (brainMode == MOVETIME_MODE || brainMode == ADJUSTTIME_MODE) {
            while ((isPondering || watch.get_elapsed_time() < thinkDelay) && brainStatus == THINKING) {
                std::this_thread::sleep_for(MONITOR_ENGINE_CLOCK);
            }
            if (brainStatus == QUITTING) return;
            brainStatus = PAUSED;
        } 
        else if (brainMode == INFINITE_MODE) {
            while (brainStatus == THINKING) {
                std::this_thread::sleep_for(MONITOR_ENGINE_CLOCK);
            }
            if (brainStatus == QUITTING) return;
            Log log;
            log << "PAUSED" << endl;
            brainStatus = PAUSED;
        }
    }
}

void Thread::brain_main_thread_loop() {
    while (true) {
        while (brainStatus != THINKING && brainStatus != QUITTING) {
            std::this_thread::sleep_for(WAITFORSTART_ENGINE_CLOCK);
        }
        
        if (brainStatus == QUITTING) return;

        bestMove.move = MOVE_NONE;
        bestMove.value = VALUE_NONE;
        ponderMove = MOVE_NONE;
        memset(&cutOffStatTable, 0, 20 * sizeof(int));
        
        int depth = 1;
        
        switch (brainMode) {
            case ADJUSTTIME_MODE:
            case MOVETIME_MODE:
                while (brainStatus == THINKING) {
                    search(currentPosition, depth);
                    depth++;
                    if (!UCI_AnalyseMode && !isPondering) {
                        if (bestMove.value > VALUE_KNOWN_WIN || 
                           ((moves->move != MOVE_NONE) && ((moves + 1)->move == MOVE_NONE))) break;
                    }
                }
                break;
            case MATE_MODE:
            case DEPTH_MODE:
                while (brainStatus == THINKING && depth <= depthLevel) {
                    search(currentPosition, depth);
                    depth++;
                }
                break;
            case NODES_MODE:
                while (brainStatus == THINKING && nbProcessedNodes < nodeMaxNumber) {
                    search(currentPosition, depth);
                    depth++;
                }
                break;
            case INFINITE_MODE:
                while (brainStatus == THINKING) {
                    search(currentPosition, depth);
                    depth++;
                }
                break;
            default:
                break;
        }
        
        stop_search();

        if (isPondering) {
            isPondering = false;
        }
        
        if (bestMove.move == MOVE_NONE) {
            cout << "bestmove -" << endl;
        } else if (Ponder) {
            cout << "bestmove " << move_to_uci(bestMove.move, currentPosition.is_chess960())
                 << " ponder " << move_to_uci(ponderMove, currentPosition.is_chess960()) << endl;
        } else {
            cout << "bestmove " << move_to_uci(bestMove.move, currentPosition.is_chess960()) << endl;
        }

        if (debugMode) {
            cout << "cut-off stat table" << endl;
            display_engine_depthstats(cutOffStatTable, depth - 1);
            display_engine_hashtable_usage();
        }
    }
}

void Thread::idle_loop() {
    while (brainStatus != QUITTING) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

// --- ThreadsManager ---

void ThreadsManager::init() {
    timer_thread = std::make_unique<Thread>(&Thread::clock_thread_loop);
    engine_thread = std::make_unique<Thread>(&Thread::brain_main_thread_loop);
    read_uci_options();
}

ThreadsManager::~ThreadsManager() {
    // Les unique_ptr libèrent la mémoire automatiquement
}

void ThreadsManager::read_uci_options() {
    const size_t requested = 1; 

    while (threads.size() < requested) {
        threads.push_back(std::make_unique<Thread>(&Thread::idle_loop));
    }

    while (threads.size() > requested) {
        threads.pop_back();
    }
}

void ThreadsManager::wake_up() const {
    for (auto& th : threads) {
        th->maxPly = 0;
        th->wake_up();
    }
}

void ThreadsManager::sleep() const {
    for (size_t i = 1; i < threads.size(); ++i) {
        threads[i]->do_sleep = true;
    }
}

void ThreadsManager::set_timer(int msec) {
    if (!timer_thread) return;
    {
        std::lock_guard<std::mutex> lk(timer_thread->sleepLock);
        timer_thread->maxPly = msec;
    }
    timer_thread->sleepCond.notify_one();
}

void ThreadsManager::wait_for_search_finished() {
    Thread* t = main_thread();
    if (!t) return;
    
    std::unique_lock<std::mutex> lk(t->sleepLock);
    t->sleepCond.wait(lk, [t] { return t->do_sleep.load(); });
}

void ThreadsManager::DisplayStatus() {
    cout << endl << "THREADS MANAGER STATUS =============== nb of threads:" << size() << endl;
}