#ifndef DAP_BASE_SYSTEM_COMMON_H
#define DAP_BASE_SYSTEM_COMMON_H

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <type_traits> // std::is_same
#include <xmmintrin.h>

//#define thread_local __thread; // osx currently does not support c++11 thread_local keyword

#ifdef _WIN32
// nothing to add
#else
#include <cfenv>
#include <pthread.h>
#include <csignal>
#include <sys/signal.h>

// flush-to-zero denormals
#ifndef _MM_DENORMALS_ZERO_MASK
#define _MM_DENORMALS_ZERO_MASK 0x0040
#define _MM_DENORMALS_ZERO_ON 0x0040
#define _MM_DENORMALS_ZERO_OFF 0x0000
#define _MM_SET_DENORMALS_ZERO_MODE(mode) \
    _mm_setcsr((_mm_getcsr() & ~_MM_DENORMALS_ZERO_MASK) | (mode))
#define _MM_GET_DENORMALS_ZERO_MODE() (_mm_getcsr() & _MM_DENORMALS_ZERO_MASK)
#endif
#endif

#if __APPLE__
#include <CoreAudio/HostTime.h>
#include <mach/mach_error.h>
#include <mach/mach_init.h>
#include <mach/thread_act.h>
#include <mach/thread_policy.h>
#endif

namespace dap
{

#ifdef __APPLE__
    inline void handleFPE(int /*signo*/, siginfo_t* info, void* /*context*/)
    {
        std::stringstream ss;
        ss << "Program received SIGFPE (floating point exception) signal with code ";
        switch (info->si_code)
        {
            case FPE_INTDIV:
                ss << "FPE_INTDIV: integer zero division";
                break;
            case FPE_INTOVF:
                ss << "FPE_INTOVF: integer overflow";
                break;
            case FPE_FLTDIV:
                ss << "FPE_FLTDIV: float zero division";
                break;
            case FPE_FLTOVF:
                ss << "FPE_FLTOVF: float overflow";
                break;
            case FPE_FLTUND:
                ss << "FPE_FLTUND: float underflow";
                break;
            case FPE_FLTRES:
                ss << "FPE_FLTRES: inexact float";
                break;
            case FPE_FLTINV:
                ss << "FPE_FLTINV: invalid float operation";
                break;
            case FPE_FLTSUB:
                ss << "FPE_FLTSUB: subscript out of range";
                break;
            default:
                ss << "Unknown";
                break;
        }
        std::cerr << ss.str() << std::endl;
        throw std::runtime_error(ss.str());
        // exit(-1);
    }
#endif

    inline void activateFloatingPointExceptions()
    {
#ifdef _WIN32
        _clearfp();
        int cw              = ::_controlfp(0, 0);
        int exceptions_mask = EM_INVALID | EM_DENORMAL | EM_ZERODIVIDE | EM_OVERFLOW | EM_UNDERFLOW;
        cw &= ~exceptions_mask;
        ::_controlfp(cw, MCW_EM);
#elif __APPLE__
        // int excepts = FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW;
        // feraiseexcept(excepts);
        int flags = _MM_GET_EXCEPTION_MASK(); // get current flags
        _MM_SET_EXCEPTION_MASK(flags & ~_MM_MASK_INVALID);
        _MM_SET_EXCEPTION_MASK(flags & ~_MM_MASK_DIV_ZERO);
        _MM_SET_EXCEPTION_MASK(flags & ~_MM_MASK_DENORM);
        _MM_SET_EXCEPTION_MASK(flags & ~_MM_MASK_OVERFLOW);
        _MM_SET_EXCEPTION_MASK(flags & ~_MM_MASK_UNDERFLOW);

        struct sigaction action; //, old_action;

        action.sa_sigaction = handleFPE; // NOLINT
        sigemptyset(&action.sa_mask);
        action.sa_flags = SA_SIGINFO;

        // sigaction (SIGFPE, NULL, &old_action);
        // if (old_action.sa_handler != SIG_IGN)
        //  sigaction (SIGFPE, &new_action, NULL);
        sigaction(SIGFPE, &action, nullptr);
#else // linux
        int excepts = FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW;
        feenableexcept(excepts);
#endif
    }
    inline int concurrent_threads()
    {
        return std::thread::hardware_concurrency();
    }

    static inline void setRealtimePriority(std::thread* thread)
    {
#if __APPLE__
        // REAL-TIME / TIME-CONSTRAINT THREAD
        pthread_t inThread                = thread ? thread->native_handle() : pthread_self(); // NOLINT
        UInt64 cycleDurationInNanoseconds = 10000000;
        thread_time_constraint_policy_data_t theTCPolicy;
        UInt64 theComputeQuanta;
        UInt64 thePeriod;
        UInt64 thePeriodNanos;

        thePeriodNanos   = cycleDurationInNanoseconds;
        theComputeQuanta = AudioConvertNanosToHostTime(thePeriodNanos * 0.15);
        thePeriod        = AudioConvertNanosToHostTime(thePeriodNanos);

        theTCPolicy.period      = thePeriod;
        theTCPolicy.computation = theComputeQuanta;
        theTCPolicy.constraint  = thePeriod;
        theTCPolicy.preemptible = 1u;
        thread_policy_set(pthread_mach_thread_np(inThread),
                          THREAD_TIME_CONSTRAINT_POLICY,
                          (thread_policy_t)&theTCPolicy, // NOLINT
                          THREAD_TIME_CONSTRAINT_POLICY_COUNT);
#endif
    }

} // namespace dap

#endif // DAP_BASE_SYSTEM_COMMON_H
