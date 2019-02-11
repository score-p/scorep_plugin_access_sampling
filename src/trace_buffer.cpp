#include <algorithm>
#include <cstring>
#include <trace_buffer.h>

namespace perf_buffer
{

#define rmb() asm volatile("lfence" ::: "memory")
#define mb() asm volatile("mfence" ::: "memory")
#define ACCESS_ONCE(x) (*(volatile typeof(x) *)&(x))

PerfRingBuffer::PerfRingBuffer (int fd)
{
    moved_ = 0;
    mask_ = BUFFERSIZE_DEFAULT * PAGE_SIZE - 1;

    base_ = mmap (NULL, (BUFFERSIZE_DEFAULT + 1) * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (base_ == MAP_FAILED)
    {
        throw std::runtime_error ("Error: Failed to allocate ring buffer.");
    }
}

uint64_t PerfRingBuffer::read_head ()
{
    struct perf_event_mmap_page *pc = static_cast<perf_event_mmap_page *> (base_);
    uint64_t head = ACCESS_ONCE (pc->data_head);
    rmb ();
    return head;
}

void PerfRingBuffer::write_tail (uint64_t tail)
{
    struct perf_event_mmap_page *pc = static_cast<perf_event_mmap_page *> (base_);
    /*
     * ensure all reads are done before we write the tail out.
     */
    mb ();
    pc->data_tail = tail;
}

void *PerfRingBuffer::read ()
{
    uint64_t old = prev_;

    unsigned char *data = static_cast<unsigned char *> (base_) + PAGE_SIZE;
    void *event = nullptr;

    uint64_t head = read_head ();

    if (old != head)
    {
        size_t size;

        event = reinterpret_cast<char *>(&data[old & mask_]);
        size = static_cast<UnknownEvent *> (event)->header.size;

        if ((old & mask_) + size != ((old + size) & mask_))
        {
            unsigned int offset = old;
            unsigned int len = size, nbytes_to_copy;
            char *dst = event_copy_;

            do
            {
                nbytes_to_copy = std::min<std::size_t> (mask_ + 1 - (offset & mask_), len);
                std::memcpy (dst, &data[offset & mask_], nbytes_to_copy);
                offset += nbytes_to_copy;
                dst += nbytes_to_copy;
                len -= nbytes_to_copy;
            } while (len);

            event = static_cast<char *> (event_copy_);
        }

        old += size;
    }
    else
    {
        write_tail (old);
    }
    prev_ = old;

    return event;
}

std::ostream &operator<< (std::ostream &os, const AccessEvent &me)
{
    auto type = (me.access_type == AccessType::LOAD) ? "load" : "store";
    os << "Address " << std::hex << me.address << std::dec << ", Time " << me.time << ", IP "
       << std::hex << me.ip << std::dec << ", AccessType " << type;
    return os;
}

AccessType accessTypeFromString (const std::string &type)
{
    if (type.find("Load") != std::string::npos)
    {
        return AccessType::LOAD;
    }
    else if (type.find("Store") != std::string::npos)
    {
        return AccessType::STORE;
    }
    else if (type.find("Prefetch") != std::string::npos)
    {
        return AccessType::PREFETCH;
    }
    else if (type.find("Exec") != std::string::npos)
    {
        return AccessType::EXEC;
    }

    return AccessType::NA;
}

AccessType accessTypeFromPerf (uint64_t mem_op)
{
    return static_cast<AccessType>(mem_op);
}

MemoryLevel memoryLevelFromPerf(const SamplingEvent & event)
{
    return static_cast<MemoryLevel>(event.data_src.mem_lvl);
}

} // namespace perf_buffer