project(access_sampling)

cmake_minimum_required(VERSION 3.8)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/common")

include(scorep_plugin_common/DefaultBuildType.cmake)

add_subdirectory(scorep_plugin_cxx_wrapper)

add_subdirectory(tracefile)

add_subdirectory(test)

add_library(access_sampling
            SHARED
            src/access_sampling.cpp
            src/pfm_wrapper.cpp
            src/ring_buffer.cpp
            src/perf_sampling.cpp
            src/utils.cpp)

set_target_properties(access_sampling PROPERTIES CXX_STANDARD 17)

target_include_directories(access_sampling PRIVATE include)
target_include_directories(access_sampling PRIVATE tracefile/include)

target_compile_options(access_sampling INTERFACE -Wall -pedantic -Wextra)

target_link_libraries(access_sampling PRIVATE Scorep::scorep-plugin-cxx boost_filesystem)
