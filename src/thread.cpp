// Mainly from Stockfish
#include <cassert>
#include <iostream>
#include <fstream>
#include "notation.h"
#include "thread.h"
#include "platform.h"
#include "engine.h"
#include "logger.h"

using namespace std;

#define WAITFORSTART_ENGINE_CLOCK 100
#define MONITOR_ENGINE_CLOCK 1000

ThreadsManager Threads; // Global object

namespace
{
extern "C"
{

    // start_routine() is the C function which is called when a new thread
    // is launched. It is a wrapper to member function pointed by start_fn.

    /*long*/ void start_routine(Thread* th)
    {
        (th->*(th->start_fn))();
        //return 0;
    }

}
}


// Thread c'tor starts a newly-created thread of execution that will call
// the idle loop function pointed by start_fn going immediately to sleep.

Thread::Thread(Fn fn)
{

    start_fn = fn;
    idx = Threads.size();

    do_sleep = (fn != &Thread::brain_main_thread_loop); // Avoid a race with start_searching()

    lock_init(sleepLock);
    cond_init(sleepCond);

    if (!thread_create(handle, (void*)start_routine, this))
    {
        std::cout << "Failed to create thread number " << idx << std::endl;
        ::exit(EXIT_FAILURE);
    }
    //std::cout <<  "Thread " << idx << " instance OK and ready to work !" << std::endl;
}


// Thread d'tor waits for thread termination before to return.
Thread::~Thread()
{

    //assert(do_sleep);

    do_exit = true; // Search must be already finished
    brainStatus = QUITTING; // Olivechess
    wake_up();

    thread_join(handle); // Wait for thread termination

    lock_destroy(sleepLock);
    cond_destroy(sleepCond);

}


// Thread::timer_loop() is where the timer thread waits maxPly milliseconds and
// then calls check_time(). If maxPly is 0 thread sleeps until is woken up.
extern void check_time();

void Thread::clock_thread_loop()
{
    // TODO: QUITTING could be cleaner
    while (brainStatus != QUITTING)
    {
        while ((brainStatus != THINKING)
                && (brainStatus != QUITTING))
        {
            Sleep(WAITFORSTART_ENGINE_CLOCK);
        }
        if(brainStatus == QUITTING) return;

        if(brainMode == MOVETIME_MODE
         || brainMode == ADJUSTTIME_MODE)
        {
            //std::cout <<  "Starting MOVETIME_MODE" << std::endl;
            // the search can be manually stopped via the command line
            // so a Sleep(thinkDelay) is not accurate enough
            // we need to check regularly if the brain was not stop by another way
            while ((isPondering || watch.get_elapsed_time() < thinkDelay)
                    && brainStatus == THINKING)
            {
                Sleep(MONITOR_ENGINE_CLOCK);
                //display_engine_monitor();
                //display_engine_movelist();
            }
            //std::cout <<  "Stopping MOVETIME_MODE" << std::endl;
            if(brainStatus == QUITTING) return;
            brainStatus = PAUSED;
        }
        else if(brainMode == INFINITE_MODE)
        {
            //std::cout <<  "Starting INFINITE_MODE" << std::endl;
            // the search can be manually stopped via the command line
            // so a Sleep(thinkDelay) is not accurate enough
            // we need to check regularly if the brain was not stop by another way
            while (brainStatus == THINKING)
            {
                Sleep(MONITOR_ENGINE_CLOCK);
                //display_engine_monitor();
                //display_engine_movelist();
            }
            //std::cout <<  "Stopping INFINITE_MODE" << std::endl;
            if(brainStatus == QUITTING) return;
            Log log;
            log << "PAUSED" << endl;
            brainStatus = PAUSED;
        }
    }
}


// Main thread - launch the different search modes
void Thread::brain_main_thread_loop()
{
    //bool traceOutputs = true;
    while (true)
    {
        // wait loop
        while ((brainStatus != THINKING)
                && (brainStatus != QUITTING))
        {
            Sleep(WAITFORSTART_ENGINE_CLOCK);
            //std::cout <<  "waiting..." << std::endl;
        }
        if(brainStatus == QUITTING) return;

        // Thread main task

        // global value
        bestMove.move = MOVE_NONE;
        bestMove.value = VALUE_NONE;
        ponderMove = MOVE_NONE;
        memset(&cutOffStatTable,0,20*sizeof(int));
        int depth = 0;
        //Position tempPos(currentPosition);
        // init alpha & beta values outside Search() to re-use a/b values updated in previous searches
        switch (brainMode)
        {
            case ADJUSTTIME_MODE:
            case MOVETIME_MODE:
                depth = 1;
                while (brainStatus == THINKING)
                {
                    search(currentPosition, depth);
                    depth++;
                    if(!UCI_AnalyseMode && !isPondering)
                    {
                        // save time for favorable mate cases or forced move
                        if(bestMove.value>VALUE_KNOWN_WIN || ((moves->move != MOVE_NONE) && ((moves+1)->move == MOVE_NONE))) break;
                    }
                }
                break;
            case MATE_MODE:
            case DEPTH_MODE:
                depth = 1;
                while (brainStatus == THINKING && depth <= depthLevel)
                {
                    search(currentPosition, depth);
                    depth++;
                }
                break;
            case NODES_MODE:
                depth = 1;
                while (brainStatus == THINKING && nbProcessedNodes < nodeMaxNumber)
                {
                    search(currentPosition, depth);
                    depth++;
                }
                break;
            case INFINITE_MODE:
                depth = 1;
                while (brainStatus == THINKING)
                {
                    search(currentPosition, depth);
                    depth++;
                }
                break;
            default:
                break;
        }
        stop_search();

        //if(debugMode) std::cout <<  "info time " << watch.GetElapsedTime() << std::endl;
        if (isPondering)
        {
        	// search loop is over but isPondering flag is still true = PonderMiss
            //Log log; log << "PonderMiss" << endl; 
            isPondering = false;
        }
        
		if(bestMove.move == MOVE_NONE)
		{
			 // Display no move
        	std::cout <<  "bestmove -" << std::endl;
		}
        if(Ponder)
        {
            cout << "bestmove " <<  move_to_uci(bestMove.move, currentPosition.is_chess960())
                 << " ponder " <<  move_to_uci(ponderMove, currentPosition.is_chess960()) << endl;
            // if(traceOutputs)
            // {
            //     std::ofstream outfile;
            //     outfile.open("olivechess.log", std::ios_base::app);
            //     outfile << "uci=>gui  " << "bestmove " <<  move_to_uci(bestMove.move, currentPosition.is_chess960())
            //             << " ponder " <<  move_to_uci(ponderMove, currentPosition.is_chess960()) << endl;
            // }
        }
        else
        {
            cout <<  "bestmove " <<  move_to_uci(bestMove.move, currentPosition.is_chess960()) << endl;
            // if(traceOutputs)
            // {
            //     std::ofstream outfile;
            //     outfile.open("olivechess.log", std::ios_base::app);
            //     outfile << "uci=>gui  " <<  "bestmove " <<  move_to_uci(bestMove.move, currentPosition.is_chess960()) << endl;
            // }
        }

        if(debugMode)
        {
            // Cut-off stats
            cout << "cut-off stat table" << endl;
            display_engine_depthstats(cutOffStatTable,depth-1);
            // hashtable perfs
            display_engine_hashtable_usage();
        }
    }
}

// Thread::wake_up() wakes up the thread, normally at the beginning of the search
// or, if "sleeping threads" is used at split time.

void Thread::wake_up()
{
    lock_grab(sleepLock);
    cond_signal(sleepCond);
    lock_release(sleepLock);
}


// init() is called at startup. Initializes lock and condition variable and
// launches requested threads sending them immediately to sleep. We cannot use
// a c'tor becuase Threads is a static object and we need a fully initialized
// engine at this point due to allocation of endgames in Thread c'tor.

void ThreadsManager::init()
{
    cond_init(sleepCond);
    lock_init(splitLock);
    timer_thread = new Thread(&Thread::clock_thread_loop);
    engine_thread = new Thread(&Thread::brain_main_thread_loop);
    //threads.push_back();

    read_uci_options();
}


// d'tor cleanly terminates the threads when the program exits.
ThreadsManager::~ThreadsManager()
{
    for (int i = 0; i < size(); i++)
        delete threads[i];

    delete engine_thread;
    delete timer_thread;
    
    lock_destroy(splitLock);
    cond_destroy(sleepCond);
}


// read_uci_options() updates internal threads parameters from the corresponding
// UCI options and creates/destroys threads to match the requested number. Thread
// objects are dynamically allocated to avoid creating in advance all possible
// threads, with included pawns and material tables, if only few are used.

void ThreadsManager::read_uci_options()
{
    int requested = 1; // For now the engine has only 1 active thread
    assert(requested > 0);

    while (size() < requested)
        threads.push_back(new Thread(&Thread::idle_loop));

    while (size() > requested)
    {
        delete threads.back();
        threads.pop_back();
    }
}


// wake_up() is called before a new search to start the threads that are waiting
// on the sleep condition and to reset maxPly. When useSleepingThreads is set
// threads will be woken up at split time.

void ThreadsManager::wake_up() const
{
    for (int i = 0; i < size(); i++)
    {
        threads[i]->maxPly = 0;
        threads[i]->do_sleep = false;

        if (!useSleepingThreads)
            threads[i]->wake_up();
    }
}


// sleep() is called after the search finishes to ask all the threads but the
// main one to go waiting on a sleep condition.

void ThreadsManager::sleep() const
{
    for (int i = 1; i < size(); i++) // Main thread will go to sleep by itself
        threads[i]->do_sleep = true; // to avoid a race with start_searching()
}


// ThreadsManager::set_timer() is used to set the timer to trigger after msec
// milliseconds. If msec is 0 then timer is stopped.

void ThreadsManager::set_timer(int msec)
{
    lock_grab(timer_thread->sleepLock);
    timer_thread->maxPly = msec;
    cond_signal(timer_thread->sleepCond); // Wake up and restart the timer
    lock_release(timer_thread->sleepLock);

}


// ThreadsManager::wait_for_search_finished() waits for main thread to go to
// sleep, this means search is finished. Then returns.

void ThreadsManager::wait_for_search_finished()
{
    Thread* t = main_thread();
    lock_grab(t->sleepLock);
    cond_signal(t->sleepCond); // In case is waiting for stop or ponderhit
    while (!t->do_sleep) cond_wait(sleepCond, t->sleepLock);
    lock_release(t->sleepLock);
}


/// Thread::idle_loop() is where the thread is parked when it has no work to do.
/// The parameter 'master_sp', if non-NULL, is a pointer to an active SplitPoint
/// object for which the thread is the master.
//void Thread::idle_loop(SplitPoint* sp_master) {
void Thread::idle_loop()
{
    while(brainStatus != QUITTING)
    {
        Sleep(500);
    }
}


void ThreadsManager::DisplayStatus()
{
    std::cout <<  std::endl << "THREADS MANAGER STATUS =============== nb of threads:" << this->size() << std::endl;
}
