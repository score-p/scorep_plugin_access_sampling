#include <trace_buffer.h>

namespace perf_buffer{

/**
 * Length of the memory mapping
 * mmap size should be 1+2^n pages,
 * first page is a metadata page
 */
static const std::size_t PAGE_SIZE = sysconf (_SC_PAGESIZE);
constexpr std::size_t BUFFERSIZE_DEFAULT = 64;
void allocate_ring_buffer (TraceBuffer * trace_buffer)
{
    trace_buffer->ring_buffer.moved = 0;
    trace_buffer->ring_buffer.mask = BUFFERSIZE_DEFAULT * PAGE_SIZE - 1;

    trace_buffer->ring_buffer.base = mmap (NULL, (BUFFERSIZE_DEFAULT + 1) * PAGE_SIZE,
                                    PROT_READ | PROT_WRITE, MAP_SHARED, trace_buffer->fd, 0);
    if (trace_buffer->ring_buffer.base == MAP_FAILED)
    {
        throw std::runtime_error("Error: Failed to allocate ring buffer.");
    }
}

} // namespace perf_buffer