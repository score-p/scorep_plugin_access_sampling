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

enum class Type : uint32_t
{
    LOAD,
    STORE,
    PREFETCH,
    EXEC,
    NA
};

enum class Level : uint32_t
{
    MEM_LVL_NA, //         Not available
    MEM_LVL_HIT, //        Hit
    MEM_LVL_MISS, //       Miss
    MEM_LVL_L1, //         Level 1 cache
    MEM_LVL_LFB, //        Line fill buffer
    MEM_LVL_L2, //         Level 2 cache
    MEM_LVL_L3, //         Level 3 cache
    MEM_LVL_LOC_RAM, //    Local DRAM
    MEM_LVL_REM_RAM1, //   Remote DRAM 1 hop
    MEM_LVL_REM_RAM2, //   Remote DRAM 2 hops
    MEM_LVL_REM_CCE1, //   Remote cache 1 hop
    MEM_LVL_REM_CCE2, //   Remote cache 2 hops
    MEM_LVL_IO, //         I/O memory
    MEM_LVL_UNC, //        Uncached memory
};

struct MemoryEvent
{
    uint64_t time = 0; // 8 Byte
    uint64_t address = 0; // 8 Byte
    uint64_t ip = 0; // 8 Byte --> unw_word_t
    Type access_type = Type::NA; // 4 Byte
    Level memory_level = Level::MEM_LVL_NA; // 4 Byte
};

struct TraceBuffer
{
    std::thread::id tid;
    static constexpr std::size_t default_buffer_size = 1e6; //=> 32M per thread and event
    boost::circular_buffer<MemoryEvent> buffer;

    TraceBuffer () : buffer (default_buffer_size, MemoryEvent ())
    {
    }
};

} // namespace perf_buffer