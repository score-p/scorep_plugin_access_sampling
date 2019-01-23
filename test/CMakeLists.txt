add_executable(test_pfm_wrapper test_pfm_wrapper.cpp "${PROJECT_SOURCE_DIR}/src/pfm_wrapper.cpp")
target_include_directories(test_pfm_wrapper PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(test_pfm_wrapper PRIVATE "${PROJECT_SOURCE_DIR}/lib/catch2")
target_link_libraries(test_pfm_wrapper pfm)
