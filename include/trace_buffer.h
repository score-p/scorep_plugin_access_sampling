#pragma once

#include <thread>

constexpr unsigned int PERF_SAMPLE_MAX_SIZE = 64;

struct PerfRingBuffer
{

    int      moved;
    void*    base;
    int      mask;
    uint64_t prev;
    char     event_copy[ PERF_SAMPLE_MAX_SIZE ] __attribute__( ( aligned( 8 ) ) );
};


struct TraceBuffer
{
    std::thread::id  tid;
    PerfRingBuffer mmp;
};