#include <algorithm>
#include <cstring>
#include <ring_buffer.h>
#include <stdexcept>

#define rmb() asm volatile("lfence" ::: "memory")
#define mb() asm volatile("mfence" ::: "memory")
#define ACCESS_ONCE(x) (*(volatile typeof(x)*)&(x))

PerfRingBuffer::PerfRingBuffer (int fd)
{
    moved_ = 0;
    mask_ = BUFFERSIZE_DEFAULT * PAGE_SIZE - 1;

    base_ = mmap (nullptr, (BUFFERSIZE_DEFAULT + 1) * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (base_ == MAP_FAILED || base_ == nullptr)
    {
        throw std::runtime_error ("Error: Failed to allocate ring buffer.");
    }
}

uint64_t
PerfRingBuffer::read_head ()
{
    auto* pc = static_cast<perf_event_mmap_page*> (base_);
    uint64_t head = ACCESS_ONCE (pc->data_head);
    rmb ();
    return head;
}

void
PerfRingBuffer::write_tail (uint64_t tail)
{
    auto* pc = static_cast<perf_event_mmap_page*> (base_);
    /*
     * ensure all reads are done before we write the tail out.
     */
    mb ();
    pc->data_tail = tail;
}

void*
PerfRingBuffer::read ()
{
    uint64_t old = prev_;

    unsigned char* data = static_cast<unsigned char*> (base_) + PAGE_SIZE;
    void* event = nullptr;

    uint64_t head = read_head ();

    if (old != head)
    {
        size_t size;

        event = reinterpret_cast<char*> (&data[old & mask_]);
        size = static_cast<UnknownEvent*> (event)->header.size;

        if ((old & mask_) + size != ((old + size) & mask_))
        {
            unsigned int offset = old;
            unsigned int len = size;
            char* dst = event_copy_;

            do
            {
                unsigned int nbytes_to_copy = std::min<std::size_t> (mask_ + 1 - (offset & mask_), len);
                std::memcpy (dst, &data[offset & mask_], nbytes_to_copy);
                offset += nbytes_to_copy;
                dst += nbytes_to_copy;
                len -= nbytes_to_copy;
            } while (len);

            event = static_cast<char*> (event_copy_);
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
