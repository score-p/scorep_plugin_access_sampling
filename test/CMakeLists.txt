add_executable(test_pfm_wrapper test_pfm_wrapper.cpp
               "${PROJECT_SOURCE_DIR}/src/pfm_wrapper.cpp"
               "${PROJECT_SOURCE_DIR}/src/utils.cpp")

target_include_directories(test_pfm_wrapper PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(test_pfm_wrapper PRIVATE "${PROJECT_SOURCE_DIR}/lib/catch2")
target_link_libraries(test_pfm_wrapper pfm)

add_executable(test_access_sampling test_utils.cpp
               "${PROJECT_SOURCE_DIR}/src/utils.cpp")

target_include_directories(test_access_sampling PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(test_access_sampling PRIVATE "${PROJECT_SOURCE_DIR}/lib/catch2")
set_target_properties(test_access_sampling PROPERTIES CXX_STANDARD 17)
target_link_libraries(test_access_sampling PRIVATE Scorep::scorep-plugin-cxx pfm)

add_executable(test_trace_buffer test_trace_buffer.cpp "${PROJECT_SOURCE_DIR}/src/trace_buffer.cpp")
target_include_directories(test_trace_buffer PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(test_trace_buffer PRIVATE "${PROJECT_SOURCE_DIR}/lib/catch2")
target_link_libraries(test_trace_buffer PRIVATE pfm)

add_executable(test_trace_file test_trace_file.cpp
               "${PROJECT_SOURCE_DIR}/src/trace_file.cpp")
target_include_directories(test_trace_file PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(test_trace_file PRIVATE "${PROJECT_SOURCE_DIR}/lib/catch2")
target_link_libraries(test_trace_file PRIVATE boost_filesystem boost_system)
set_target_properties(test_trace_file PROPERTIES CXX_STANDARD 17)