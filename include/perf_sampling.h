#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <thread>

extern "C"
{
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/syscall.h>
}

#include <pfm_wrapper.h>
#include <trace_buffer.h>

using EventBufferPtr = std::shared_ptr<perf_buffer::EventBuffer>;
using RingBufferMap = std::map<int, perf_buffer::PerfRingBuffer>;
using SignalHandlerFuncPtr = void (*) (int, siginfo_t *, void *);

class PerfSampling
{
    public:
    PerfSampling();
    void event_open (PerfEventAttribute *attr);

    void initialize_signal_handler ();
    EventBufferPtr get_event_buffer();

    static void signal_handler (int signal, siginfo_t *info, void *context);

    static thread_local RingBufferMap ring_buffers_;
    static thread_local EventBufferPtr event_data_;
};