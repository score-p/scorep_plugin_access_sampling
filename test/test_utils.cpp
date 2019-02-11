#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>

#include <utils.h>

TEST_CASE ("utils::parse_metric")
{
    auto [metric, period] = parse_metric("Load@600");
    REQUIRE(metric == "Load");
    REQUIRE(period == 600);

    REQUIRE_THROWS(parse_metric("Load@600@foo"));
    REQUIRE_THROWS(parse_metric("Load"));
}
