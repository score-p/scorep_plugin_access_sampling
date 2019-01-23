#pragma once

#include <map>
#include <memory>

#include <trace_buffer.h>
#include <pfm_wrapper.h>

using BufferPtr          = std::shared_ptr<TraceBuffer>;
using PerfFd             = int;

struct ThreadData
{
    bool initialized;
    std::map<PerfFd, BufferPtr> buffers;
};

static thread_local ThreadData thread_data_;

class PerfSampling
{
    public:
    void event_open (BufferPtr buffer, PerfEventAttribute attr);
};