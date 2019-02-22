#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>

#include <boost/filesystem.hpp>
#include <cstdlib>
#include <utils.h>

TEST_CASE ("utils::parse_metric")
{
    auto [metric, period] = parse_metric ("Load@600");
    REQUIRE (metric == "Load");
    REQUIRE (period == 600);

    REQUIRE_THROWS (parse_metric ("Load@600@foo"));
    REQUIRE_THROWS (parse_metric ("Load"));
}

TEST_CASE ("utils::create_trace_directory")
{
    namespace bf = boost::filesystem;

    auto path = create_trace_directory ();
    REQUIRE (bf::is_directory (path));
    REQUIRE (bf::exists (path));
    REQUIRE (path == bf::current_path ());

    const char *new_path = "/tmp/foobarbaz";
    setenv ("AS_TRACE_PATH", new_path, true);

    path = create_trace_directory ();
    REQUIRE (bf::is_directory (path));
    REQUIRE (bf::exists (path));
    REQUIRE (path == bf::path (new_path));

    path.append ("blubb.txt");
    bf::ofstream path_stream (path);
    path_stream.close ();

    REQUIRE (bf::exists (path));
    REQUIRE (bf::is_regular_file (path));
}