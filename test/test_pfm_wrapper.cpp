#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
#include <pfm_wrapper.h>

TEST_CASE ("Pfm Wrapper", "[metric_is_supported]")
{
    PfmWrapper pw;
    REQUIRE(pw.metric_is_supported("Load") == true );
    REQUIRE(pw.metric_is_supported("Store") == true );
    REQUIRE(pw.metric_is_supported("FOO") == false );
}