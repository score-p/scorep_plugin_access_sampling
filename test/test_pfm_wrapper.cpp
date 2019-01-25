#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
#include <pfm_wrapper.h>

TEST_CASE ("PfmWrapper::metric_is_supported")
{
    PfmWrapper pw;
    REQUIRE(pw.metric_is_supported("Load") == true );
    REQUIRE(pw.metric_is_supported("load") == true );
    REQUIRE(pw.metric_is_supported("Store") == true );
    REQUIRE(pw.metric_is_supported("FOO") == false );
}

TEST_CASE("PfmWrapper::get_perf_event")
{
    PfmWrapper pw;
    PerfEventAttribute perf_attr;

    REQUIRE_NOTHROW(pw.get_perf_event ("load", 300, &perf_attr));
    REQUIRE_NOTHROW(pw.get_perf_event ("store", 300, &perf_attr));
    REQUIRE_THROWS(pw.get_perf_event ("FLUPP", 300, &perf_attr));
}