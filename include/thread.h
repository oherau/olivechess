// Mainly from Stockfish

#if !defined(THREAD_H_INCLUDED)
#define THREAD_H_INCLUDED

#include <vector>
#include <queue>

#include "engine.h" // <-- to allow access to main Engine object
#include "platform.h"
#include "position.h"


const int MAX_THREADS = 32;
const int MAX_SPLITPOINTS_PER_THREAD = 8;

class Thread;

/*
struct SplitPoint {

  // Const data after split point has been setup
  const Position* pos;
  //const Search::Stack* ss;
  Depth depth;
  Value beta;
  int nodeType;
  Thread* master;
  Move threatMove;

  // Const pointers to shared data
  //MovePicker* mp;
  //SplitPoint* parent;

  // Shared data
  Lock lock;
  volatile uint64_t slavesMask;
  volatile int64_t nodes;
  volatile Value alpha;
  volatile Value bestValue;
  volatile Move bestMove;
  volatile int moveCount;
  volatile bool cutoff;
};
*/

/// Thread struct keeps together all the thread related stuff like locks, state
/// and especially split points. We also use per-thread pawn and material hash
/// tables so that once we get a pointer to an entry its life time is unlimited
/// and we don't have to care about someone changing the entry under our feet.

class Thread
{

    Thread(const Thread&);            // Only declared to disable the default ones
    Thread& operator=(const Thread&); // that are not suitable in this case.

    typedef void (Thread::* Fn) ();

public:
    Thread(Fn fn);
    ~Thread();

    void wake_up();
    bool cutoff_occurred() const;
    bool is_available_to(Thread* master) const;
    void idle_loop();// void idle_loop(){ idle_loop(NULL); } // Hack to allow storing in start_fn
    void brain_main_thread_loop();
    void clock_thread_loop();

    void wait_for_stop_or_ponderhit();
    int idx;
    int maxPly;
    Lock sleepLock;
    WaitCondition sleepCond;
    NativeHandle handle;
    Fn start_fn;
    volatile bool is_searching;
    volatile bool do_sleep;
    volatile bool do_exit;
};


/// ThreadsManager class handles all the threads related stuff like init, starting,
/// parking and, the most important, launching a slave thread at a split point.
/// All the access to shared thread data is done through this class.

class ThreadsManager
{
    //As long as the single ThreadsManager object is defined as a global we don't
    //  need to explicitly initialize to zero its data members because variables with
    //  static storage duration are automatically set to zero before enter main()

public:
    void init(); // No c'tor becuase Threads is static and we need engine initialized
    ~ThreadsManager();

    void DisplayStatus();

    Thread& operator[](int id)
    {
        return *threads[id];
    }
    int size() const
    {
        return (int)threads.size();
    }
    Thread* main_thread()
    {
        return threads[0];
    }

    void wake_up() const;
    void sleep() const;
    void read_uci_options();
    void set_timer(int msec);
    void wait_for_search_finished();

private:
    friend class Thread;

    std::vector<Thread*> threads;
    Thread* timer_thread;
    Thread* engine_thread;
    Lock splitLock;
    WaitCondition sleepCond;
    //Depth minimumSplitDepth;
    int maxThreadsPerSplitPoint;
    bool useSleepingThreads;
};

extern ThreadsManager Threads;


#endif // !defined(THREAD_H_INCLUDED)
