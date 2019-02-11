#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
#include <iostream>
#include <trace_buffer.h>

TEST_CASE ("trace_buffer::memoryLevelFromPerf")
{
    SamplingEvent event;
    event.data_src.mem_lvl = PERF_MEM_LVL_L1;
    event.data_src.mem_lvl_num = PERF_MEM_LVLNUM_L1;

    uint64_t t = event.data_src.val >> PERF_MEM_LVL_SHIFT;
    bool f = t & PERF_MEM_LVL_L1;
    REQUIRE(f);

    REQUIRE(memoryLevelFromPerf(event) == MemoryLevel::MEM_LVL_L1);
}