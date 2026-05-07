/*
  FROM Stockfish
*/

#if !defined(PLATFORM_H_INCLUDED)
#define PLATFORM_H_INCLUDED

#if defined(_MSC_VER)

// Disable some silly and noisy warning from MSVC compiler
#pragma warning(disable: 4127) // Conditional expression is constant
#pragma warning(disable: 4146) // Unary minus operator applied to unsigned type
#pragma warning(disable: 4800) // Forcing value to bool 'true' or 'false'
#pragma warning(disable: 4996) // Function _ftime() may be unsafe

// MSVC does not support <inttypes.h>
typedef   signed __int8    int8_t;
typedef unsigned __int8   uint8_t;
typedef   signed __int16  int16_t;
typedef unsigned __int16 uint16_t;
typedef   signed __int32  int32_t;
typedef unsigned __int32 uint32_t;
typedef   signed __int64  int64_t;
typedef unsigned __int64 uint64_t;

#else
#  include <inttypes.h>
#endif

#if !defined(_WIN32) && !defined(_WIN64) // Linux - Unix

#  include <sys/time.h>
typedef timeval sys_time_t;

inline void system_time(sys_time_t* t)
{
    gettimeofday(t, NULL);
}
inline uint64_t time_to_msec(const sys_time_t& t)
{
    return t.tv_sec * 1000LL + t.tv_usec / 1000;
}

#include <unistd.h>
inline void Sleep(int millisec)
{
    usleep(millisec*1000);
}

#  include <pthread.h>
typedef pthread_mutex_t Lock;
typedef pthread_cond_t WaitCondition;
typedef pthread_t NativeHandle;
typedef void*(*pt_start_fn)(void*);

#  define lock_init(x) pthread_mutex_init(&(x), NULL)
#  define lock_grab(x) pthread_mutex_lock(&(x))
#  define lock_release(x) pthread_mutex_unlock(&(x))
#  define lock_destroy(x) pthread_mutex_destroy(&(x))
#  define cond_destroy(x) pthread_cond_destroy(&(x))
#  define cond_init(x) pthread_cond_init(&(x), NULL)
#  define cond_signal(x) pthread_cond_signal(&(x))
#  define cond_wait(x,y) pthread_cond_wait(&(x),&(y))
#  define cond_timedwait(x,y,z) pthread_cond_timedwait(&(x),&(y),z)
#  define thread_create(x,f,t) !pthread_create(&(x),NULL,(pt_start_fn)f,t)
#  define thread_join(x) pthread_join(x, NULL)

#else // Windows and MinGW

#  include <sys/timeb.h>
typedef _timeb sys_time_t;

inline void system_time(sys_time_t* t)
{
    _ftime(t);
}
inline uint64_t time_to_msec(const sys_time_t& t)
{
    return t.time * 1000LL + t.millitm;
}

#if !defined(NOMINMAX)
#  define NOMINMAX // disable macros min() and max()
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#undef NOMINMAX

// We use critical sections on Windows to support Windows XP and older versions,
// unfortunatly cond_wait() is racy between lock_release() and WaitForSingleObject()
// but apart from this they have the same speed performance of SRW locks.
typedef CRITICAL_SECTION Lock;
typedef HANDLE WaitCondition;
typedef HANDLE NativeHandle;

#  define lock_init(x) InitializeCriticalSection(&(x))
#  define lock_grab(x) EnterCriticalSection(&(x))
#  define lock_release(x) LeaveCriticalSection(&(x))
#  define lock_destroy(x) DeleteCriticalSection(&(x))
#  define cond_init(x) { x = CreateEvent(0, FALSE, FALSE, 0); }
#  define cond_destroy(x) CloseHandle(x)
#  define cond_signal(x) SetEvent(x)
#  define cond_wait(x,y) { lock_release(y); WaitForSingleObject(x, INFINITE); lock_grab(y); }
#  define cond_timedwait(x,y,z) { lock_release(y); WaitForSingleObject(x,z); lock_grab(y); }
#  define thread_create(x,f,t) (x = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)f,t,0,NULL), x != NULL)
#  define thread_join(x) { WaitForSingleObject(x, INFINITE); CloseHandle(x); }

#endif

#endif // !defined(PLATFORM_H_INCLUDED)
