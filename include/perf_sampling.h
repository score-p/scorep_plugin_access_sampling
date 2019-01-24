#pragma once

#include <memory>
#include <mutex>
#include <vector>


#include <pfm_wrapper.h>
#include <trace_buffer.h>

using BufferPtr = std::shared_ptr<perf_buffer::TraceBuffer>;

class ThreadData
{
    public:
    ThreadData () : event_count_ (0)
    {
    }
    void add_buffer (BufferPtr buffer)
    {
        buffers_[event_count_++] = buffer;
    }

    private:
    unsigned int event_count_;
    BufferPtr buffers_[EVENT_COUNT_MAX];
};

class PerfSampling
{
    public:
    void event_open (BufferPtr buffer, PerfEventAttribute attr);
    std::size_t event_open (PerfEventAttribute *attr);

    private:
    std::mutex buffer_mutex_;
    std::vector<BufferPtr> thread_buffers_;
};