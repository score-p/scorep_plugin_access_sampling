#pragma once

#include <thread>

extern "C"
{
#include <sys/mman.h>
#include <unistd.h>
}

namespace perf_buffer
{
constexpr std::size_t PERF_SAMPLE_MAX_SIZE = 64;

struct PerfRingBuffer
{
    int moved;
    void *base;
    int mask;
    uint64_t prev;
    char event_copy[PERF_SAMPLE_MAX_SIZE] __attribute__ ((aligned (8)));
};


struct TraceBuffer
{
    std::thread::id tid;
    int fd;
    PerfRingBuffer ring_buffer;
};

void allocate_ring_buffer (TraceBuffer * trace_buffer);

} // namespace perf_buffer