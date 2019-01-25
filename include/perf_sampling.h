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
    ThreadData () : event_count_ (0)
    {
    }
    // TODO rename to add_event_buffer
    void add_buffer (BufferPtr buffer)
    {
        buffers_[event_count_++] = buffer;
    }

    perf_buffer::TraceBuffer * get_trace_buffer(int fd)
    {
        for(int i = 0; i < event_count_; i++)
        {
            if (buffers_[i]->fd == fd)
            {
                return buffers_[i].get();
            }
        }
        return nullptr;
    }

    private:
    unsigned int event_count_;
    BufferPtr buffers_[EVENT_COUNT_MAX];
};

class PerfSampling
{
    public:
    PerfSampling ();
    void event_open (PerfEventAttribute *attr, BufferPtr trace_buffer);

    private:
    void initialize_signal_handler ();
};