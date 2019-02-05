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
    moved = 0;
    mask = BUFFERSIZE_DEFAULT * PAGE_SIZE - 1;

    base = mmap (NULL, (BUFFERSIZE_DEFAULT + 1) * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (base == MAP_FAILED)
    {
        throw std::runtime_error ("Error: Failed to allocate ring buffer.");
    }
}

uint64_t PerfRingBuffer::read_head ()
{
    struct perf_event_mmap_page *pc = static_cast<perf_event_mmap_page *> (base);
    uint64_t head = ACCESS_ONCE (pc->data_head);
    rmb ();
    return head;
}

void PerfRingBuffer::write_tail (uint64_t tail)
{
    struct perf_event_mmap_page *pc = static_cast<perf_event_mmap_page *> (base);
    /*
     * ensure all reads are done before we write the tail out.
     */
    mb ();
    pc->data_tail = tail;
}

void *PerfRingBuffer::read ()
{
    uint64_t old = prev;
    // TODO convert into char*
    unsigned char *data = static_cast<unsigned char *> (base) + PAGE_SIZE;
    void *event = NULL;

    uint64_t head = read_head ();

    if (old != head)
    {
        size_t size;

        event = (char *)&data[old & mask];
        size = static_cast<UnknownEvent *> (event)->header.size;

        if ((old & mask) + size != ((old + size) & mask))
        {
            unsigned int offset = old;
            unsigned int len = size, cpy;
            char *dst = event_copy;

            do
            {
                cpy = std::min (mask + 1 - (offset & mask), len);
                std::memcpy (dst, &data[offset & mask], cpy);
                offset += cpy;
                dst += cpy;
                len -= cpy;
            } while (len);

            event = static_cast<char *> (event_copy);
        }

        old += size;
    }
    else
    {
        write_tail (old);
    }
    prev = old;

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
    if (type == "store")
    {
        return AccessType::LOAD;
    }
    else if (type == "load")
    {
        return AccessType::STORE;
    }
    else if (type == "PREFETCH")
    {
        return AccessType::PREFETCH;
    }
    else if (type == "EXEC")
    {
        return AccessType::EXEC;
    }

    return AccessType::NA;
}
}

} // namespace perf_buffer