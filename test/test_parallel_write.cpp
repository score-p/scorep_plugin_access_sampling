#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <boost/filesystem.hpp>
#include <catch.hpp>
#include <ring_buffer.h>
#include <trace_file.h>
#include <utils.h>
#define private public
#include <access_sampling.h>
#undef private

namespace bf = boost::filesystem;

inline bf::path
trace_path (ThreadId tid)
{
    std::stringstream ss;
    ss << "trace." << convert_thread_id (tid) << ".bin";
    return bf::path (ss.str ());
}

bool
operator== (const AccessEvent& a1, const AccessEvent& a2)
{
    return a1.time == a2.time && a1.access_type == a2.access_type && a1.address == a2.address &&
           a1.ip == a2.ip && a1.memory_level == a2.memory_level;
}

TEST_CASE ("access_sampling::stop")
{
    ThreadId id1 (1000);
    ThreadId id2 (400000000);

    auto p1 = std::make_shared<EventRingBuffer> (1024);
    auto p2 = std::make_shared<EventRingBuffer> (1024);
    auto a1 = AccessEvent (1, 10, 100, AccessType::LOAD, MemoryLevel::MEM_LVL_L1);
    auto a2 = AccessEvent (2, 11, 101, AccessType::STORE, MemoryLevel::MEM_LVL_L2);
    auto a3 = AccessEvent (10, 100, 1000, AccessType::LOAD, MemoryLevel::MEM_LVL_L3);
    auto a4 = AccessEvent (11, 101, 1001, AccessType::STORE, MemoryLevel::MEM_LVL_LOC_RAM);

    p1->append (a1);
    p1->append (a2);
    p2->append (a3);
    p2->append (a4);
    {
        access_sampling as;
        as.thread_event_buffers_[id1] = p1;
        as.thread_event_buffers_[id2] = p2;
        REQUIRE (as.thread_event_buffers_.size () == 2);
        as.stop ();
    }

    auto t1 = trace_path (id1);
    REQUIRE (bf::exists (t1));

    auto t2 = trace_path (id2);
    REQUIRE (bf::exists (t2));
    TraceFile tf1 (t1, TraceFileMode::READ);
    TraceFile tf2 (t2, TraceFileMode::READ);

    auto event_buffer1 = tf1.read<std::vector<AccessEvent>> ();
    auto event_buffer2 = tf2.read<std::vector<AccessEvent>> ();

    REQUIRE (event_buffer1.size () == 2);
    REQUIRE (event_buffer2.size () == 2);

    REQUIRE (event_buffer1[0] == a1);
    REQUIRE (event_buffer1[1] == a2);
    REQUIRE (event_buffer2[0] == a3);
    REQUIRE (event_buffer2[1] == a4);

    REQUIRE (bf::remove (t1));
    REQUIRE (bf::remove (t2));
}