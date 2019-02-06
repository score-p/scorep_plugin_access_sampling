#pragma once

#include <boost/circular_buffer.hpp>
#include <thread>

extern "C"
{
#include <linux/perf_event.h>
#include <sys/mman.h>
#include <unistd.h>
}
namespace perf_buffer
{

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

struct PerfRingBuffer
{
    public:
    explicit PerfRingBuffer (int fd);

    void *read ();

    private:
    inline void write_tail (uint64_t tail);
    inline uint64_t read_head ();

    private:
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

enum class AccessType : uint32_t
{
    LOAD = PERF_MEM_OP_LOAD,
    STORE = PERF_MEM_OP_STORE,
    PREFETCH = PERF_MEM_OP_PFETCH,
    EXEC = PERF_MEM_OP_EXEC,
    NA = PERF_MEM_OP_NA,
};

AccessType accessTypeFromString (const std::string &type);
AccessType accessTypeFromPerf (uint64_t mem_op);

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

struct AccessEvent
{
    AccessEvent ()
    {
    }
    AccessEvent (uint64_t t, uint64_t a, uint64_t i, AccessType at, Level l)
    : time (t), address (a), ip (i), access_type (at), memory_level (l)
    {
    }
    uint64_t time = 0; // 8 Byte
    uint64_t address = 0; // 8 Byte
    uint64_t ip = 0; // 8 Byte --> unw_word_t
    AccessType access_type = AccessType::NA; // 4 Byte
    Level memory_level = Level::MEM_LVL_NA; // 4 Byte
};
std::ostream &operator<< (std::ostream &os, const AccessEvent &me);

struct EventBuffer
{
    uint64_t number_of_accesses = 0;
    std::thread::id tid;
    static constexpr std::size_t default_buffer_size = 5e6; //=> 32M per thread and event
    boost::circular_buffer<AccessEvent> data;

    EventBuffer () : data (default_buffer_size, AccessEvent())
    {
    }
};

} // namespace perf_buffer