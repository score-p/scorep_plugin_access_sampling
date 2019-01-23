add_executable(test_perf_event test_perf_event.cpp)
target_include_directories(test_perf_event PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(test_perf_event PRIVATE "${PROJECT_SOURCE_DIR}/lib/catch2")
target_link_libraries(test_perf_event pfm)
