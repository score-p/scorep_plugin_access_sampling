add_executable(test_pfm_wrapper test_pfm_wrapper.cpp
               "${PROJECT_SOURCE_DIR}/src/pfm_wrapper.cpp"
               "${PROJECT_SOURCE_DIR}/src/utils.cpp")
target_include_directories(test_pfm_wrapper PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(test_pfm_wrapper PRIVATE "${PROJECT_SOURCE_DIR}/lib/catch2")
target_link_libraries(test_pfm_wrapper pfm boost_filesystem boost_system)

add_executable(test_utils test_utils.cpp
               "${PROJECT_SOURCE_DIR}/src/utils.cpp")
target_include_directories(test_utils PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(test_utils PRIVATE "${PROJECT_SOURCE_DIR}/lib/catch2")
set_target_properties(test_utils PROPERTIES CXX_STANDARD 17)
target_link_libraries(test_utils PRIVATE Scorep::scorep-plugin-cxx pfm boost_filesystem boost_system)

add_executable(test_trace_buffer test_trace_buffer.cpp "${PROJECT_SOURCE_DIR}/src/trace_buffer.cpp")
target_include_directories(test_trace_buffer PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(test_trace_buffer PRIVATE "${PROJECT_SOURCE_DIR}/lib/catch2")
target_link_libraries(test_trace_buffer PRIVATE pfm)

add_executable(test_trace_file test_trace_file.cpp
               "${PROJECT_SOURCE_DIR}/src/trace_file.cpp"
               "${PROJECT_SOURCE_DIR}/src/utils.cpp")
target_include_directories(test_trace_file PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(test_trace_file PRIVATE "${PROJECT_SOURCE_DIR}/lib/catch2")
target_link_libraries(test_trace_file PRIVATE boost_filesystem boost_system)
set_target_properties(test_trace_file PROPERTIES CXX_STANDARD 17)