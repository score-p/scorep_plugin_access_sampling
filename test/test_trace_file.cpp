#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <boost/filesystem.hpp>
#include <catch.hpp>
#include <thread>

#include <trace_buffer.h>

#define private public
#include <trace_file.h>
#undef private

namespace bf = boost::filesystem;

TEST_CASE ("tracefile::create")
{
    {
        TraceFile tf ("./foo", TraceFileMode::WRITE);
    }
    bf::path p{ "./foo" };
    REQUIRE (bf::is_regular_file (p));
}

TEST_CASE ("tracefile::metadata::rw")
{
    TraceFile::TraceMetaData md_read;
    TraceFile::TraceMetaData md_write;
    md_write.access_count = 42;
    md_write.tid = 1337;
    {
        TraceFile tf ("./foo", TraceFileMode::WRITE);
        tf.write_meta_data (md_write);
    }
    {
        TraceFile tf ("./foo", TraceFileMode::READ);
        tf.read_meta_data(&md_read);
    }
    REQUIRE(md_write.tid == md_read.tid);
    REQUIRE(md_write.access_count == md_read.access_count);
}

TEST_CASE ("tracefile::simple")
{
    AccessEvent ae1 (1, 0x1, 10, AccessType::STORE, MemoryLevel::MEM_LVL_L1);
    AccessEvent ae2 (2, 0x2, 20, AccessType::LOAD, MemoryLevel::MEM_LVL_L2);
    EventBuffer eb (2);

    eb.tid = std::this_thread::get_id ();
    eb.add(1, 0x1, 10, AccessType::STORE, MemoryLevel::MEM_LVL_L1);
    eb.add(2, 0x2, 20, AccessType::LOAD, MemoryLevel::MEM_LVL_L2);

    {
        TraceFile tf ("./foo", TraceFileMode::WRITE);
        tf.write (eb);
    }

    TraceFile tf ("./foo", TraceFileMode::READ);
    auto result = tf.read ();

    REQUIRE (result.size () == 2);

    REQUIRE (result[0].time == ae1.time);
    REQUIRE (result[0].address == ae1.address);
    REQUIRE (result[0].ip == ae1.ip);
    REQUIRE (result[0].access_type == ae1.access_type);
    REQUIRE (result[0].memory_level == ae1.memory_level);

    REQUIRE (result[1].time == ae2.time);
    REQUIRE (result[1].address == ae2.address);
    REQUIRE (result[1].ip == ae2.ip);
    REQUIRE (result[1].access_type == ae2.access_type);
    REQUIRE (result[1].memory_level == ae2.memory_level);
}
