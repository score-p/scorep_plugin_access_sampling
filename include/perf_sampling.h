#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <thread>

#include <pfm_wrapper.h>
#include <trace_buffer.h>

using BufferPtr = std::shared_ptr<perf_buffer::TraceBuffer>;

class ThreadData
{
    public:
    ThreadData ()
    {
    }

    void add_buffer (int fd, BufferPtr buffer)
    {
        buffers.insert_or_assign(fd, buffer);
    }
    /*
     * TODO buffers[fd] could be dangerous in the signal context
     * if it throws an exception?
     * Take a look at find?
     */
    perf_buffer::TraceBuffer * get_trace_buffer(int fd)
    {
        return buffers[fd].get();
    }

    private:
    // unsigned int event_count_;
    // BufferPtr buffers_[EVENT_COUNT_MAX];
    std::map<int, BufferPtr> buffers;
};

class PerfSampling
{
    public:
    PerfSampling ();
    void event_open (PerfEventAttribute *attr, BufferPtr trace_buffer);

    private:
    void initialize_signal_handler ();
};