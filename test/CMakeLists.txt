set(CMAKE_CXX_STANDARD 17)

add_executable(test_pfm_wrapper test_pfm_wrapper.cpp
               "${PROJECT_SOURCE_DIR}/src/pfm_wrapper.cpp"
               "${PROJECT_SOURCE_DIR}/src/utils.cpp")

target_include_directories(test_pfm_wrapper PRIVATE 
  "${PROJECT_SOURCE_DIR}/include" 
  ${Boost_INCLUDE_DIRS}
  "${PFM_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/lib/catch2")

 target_link_libraries(test_pfm_wrapper ${PFM_LIBRARIES} ${Boost_LIBRARIES})

add_executable(test_utils test_utils.cpp
               "${PROJECT_SOURCE_DIR}/src/utils.cpp")

target_include_directories(test_utils PRIVATE 
  "${PROJECT_SOURCE_DIR}/include" 
  ${Boost_INCLUDE_DIRS}
  "${PFM_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/lib/catch2")

 target_link_libraries(test_utils PRIVATE Scorep::scorep-plugin-cxx ${PFM_LIBRARIES} ${Boost_LIBRARIES})

add_executable(test_parallel_write test_parallel_write.cpp
               "${PROJECT_SOURCE_DIR}/src/access_sampling.cpp"
               "${PROJECT_SOURCE_DIR}/src/perf_sampling.cpp"
               "${PROJECT_SOURCE_DIR}/src/pfm_wrapper.cpp"
               "${PROJECT_SOURCE_DIR}/src/ring_buffer.cpp"
               "${PROJECT_SOURCE_DIR}/src/utils.cpp")

target_include_directories(test_parallel_write PRIVATE 
  "${PROJECT_SOURCE_DIR}/include" 
  ${Boost_INCLUDE_DIRS}
  "${PFM_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/tracefile/include"
  "${PROJECT_SOURCE_DIR}/lib/catch2")

 target_link_libraries(test_parallel_write PRIVATE Scorep::scorep-plugin-cxx ${Boost_LIBRARIES} pthread ${PFM_LIBRARIES})

add_executable(test_integration test_integration.cpp
               "${PROJECT_SOURCE_DIR}/src/access_sampling.cpp"
               "${PROJECT_SOURCE_DIR}/src/perf_sampling.cpp"
               "${PROJECT_SOURCE_DIR}/src/pfm_wrapper.cpp"
               "${PROJECT_SOURCE_DIR}/src/ring_buffer.cpp"
               "${PROJECT_SOURCE_DIR}/src/utils.cpp")

target_include_directories(test_integration PRIVATE 
  "${PROJECT_SOURCE_DIR}/include" 
  "${Boost_INCLUDE_DIRS}"
  "${PFM_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/tracefile/include"
  "${PROJECT_SOURCE_DIR}/lib/catch2")

 target_link_libraries(test_integration PRIVATE Scorep::scorep-plugin-cxx ${Boost_LIBRARIES} pthread ${PFM_LIBRARIES})

