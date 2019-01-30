#pragma once

#include <boost/circular_buffer.hpp>
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
    explicit PerfRingBuffer (int fd);
    int moved;
    void *base;
    int mask;
    uint64_t prev;
    char event_copy[PERF_SAMPLE_MAX_SIZE] __attribute__ ((aligned (8)));

    /**
     * Length of the memory mapping
     * mmap size should be 1+2^n pages,
     * first page is a metadata page
     */
    const std::size_t PAGE_SIZE = sysconf (_SC_PAGESIZE);
    static constexpr std::size_t BUFFERSIZE_DEFAULT = 64;
};


struct TraceBuffer
{
    std::thread::id tid;
    int fd;
    PerfRingBuffer ring_buffer;
};

} // namespace perf_buffer