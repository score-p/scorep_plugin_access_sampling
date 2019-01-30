#include <trace_buffer.h>

namespace perf_buffer
{

PerfRingBuffer::PerfRingBuffer (int fd)
{
    moved = 0;
    mask = BUFFERSIZE_DEFAULT * PAGE_SIZE - 1;

    base = mmap (NULL, (BUFFERSIZE_DEFAULT + 1) * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (base == MAP_FAILED)
    {
        throw std::runtime_error ("Error: Failed to allocate ring buffer.");
    }
}

} // namespace perf_buffer