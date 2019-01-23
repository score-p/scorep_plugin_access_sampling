#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
#include <perf_event.h>

TEST_CASE ("Perf Event", "[metric_is_supported]")
{
    PerfEvent pe;
    REQUIRE(pe.metric_is_supported("Load") == true );
    REQUIRE(pe.metric_is_supported("Store") == true );
    REQUIRE(pe.metric_is_supported("FOO") == false );
}