project(access_sampling)

cmake_minimum_required(VERSION 3.8)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/common")

find_package(PFM REQUIRED)

find_package(Boost COMPONENTS filesystem system)

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

target_include_directories(access_sampling PRIVATE 
  include 
  ${Boost_INCLUDE_DIRS}
  ${PFM_INCLUDE_DIRS}
  tracefile/include)

target_compile_options(access_sampling INTERFACE -Wall -pedantic -Wextra)

target_link_libraries(access_sampling PRIVATE 
  Scorep::scorep-plugin-cxx 
  ${Boost_LIBRARIES}
  ${PFM_LIBRARIES})

install(TARGETS access_sampling DESTINATION lib)

