#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>

#define private public

#include <access_sampling.h>

TEST_CASE ("access_sampling::parse_metric")
{
    access_sampling as;
    auto [metric, period] = as.parse_metric("Load@600");
    REQUIRE(metric == "Load");
    REQUIRE(period == 600);

    REQUIRE_THROWS(as.parse_metric("Load@600@foo"));
    REQUIRE_THROWS(as.parse_metric("Load"));
}
