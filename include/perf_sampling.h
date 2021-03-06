#pragma once

#include <memory>
#include <mutex>
#include <thread>
#include <vector>

extern "C"
{
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/syscall.h>
}

#include <pfm_wrapper.h>
#include <ring_buffer.h>
#include <trace_events.h>

using EventBufferPtr = std::shared_ptr<EventRingBuffer>;
using RingBufferMap = std::map<int, PerfRingBuffer>;
using SignalHandlerFuncPtr = void (*) (int, siginfo_t*, void*);

class PerfSampling
{
    public:
    PerfSampling ();

    void
    event_open (PerfEventAttribute* attr);

    void
    initialize_signal_handler ();

    void
    enable ();

    void
    disable ();

    void
    set_event_buffer (EventBufferPtr event_buffer);

    private:
    static inline void
    process_events (PerfRingBuffer* ring_buffer);
    static void
    signal_handler (int signal, siginfo_t* info, void* context);

    static inline void
    enable (int fd);

    static inline void
    disable (int fd);

    private:
    static thread_local RingBufferMap ring_buffers_;
    static thread_local EventBufferPtr event_data_;
};