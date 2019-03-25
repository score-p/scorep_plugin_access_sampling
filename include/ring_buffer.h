#pragma once

#include <cstdint>

extern "C"
{
#include <linux/perf_event.h>
#include <sys/mman.h>
#include <unistd.h>
}

using EventHeader = struct perf_event_header;

constexpr std::size_t PERF_SAMPLE_MAX_SIZE = 64;

struct SamplingEvent
{
    EventHeader header;
    uint64_t ip;
    uint64_t addr;
    union perf_mem_data_src data_src;
};

struct UnknownEvent
{
    EventHeader header;
};

class PerfRingBuffer
{
    public:
    explicit PerfRingBuffer (int fd);
    ~PerfRingBuffer () = default;
    PerfRingBuffer (const PerfRingBuffer& other) = delete;

    PerfRingBuffer&
    operator= (const PerfRingBuffer&) = delete;

    PerfRingBuffer (PerfRingBuffer&& other) = default;

    void*
    read ();

    private:
    inline void
    write_tail (uint64_t tail);
    inline uint64_t
    read_head ();

    private:
    int moved_ = 0;
    void* base_ = nullptr;
    int mask_ = 0;
    uint64_t prev_ = 0;
    char event_copy_[PERF_SAMPLE_MAX_SIZE] __attribute__ ((aligned (8))) = { 0 };

    /**
     * Length of the memory mapping
     * mmap size should be 1+2^n pages,
     * first page is a metadata page
     */
    const std::size_t PAGE_SIZE = sysconf (_SC_PAGESIZE);
    static constexpr std::size_t BUFFERSIZE_DEFAULT = 64;
};
