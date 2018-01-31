
# --- Here's the GoogleTest way method
# Define helper functions and macros used by Google Test.
#include(~/Downloads/googletest-release-1.8.0/googletest/cmake/internal_utils.cmake)
#enable_testing()
#cxx_executable(ord_tests . gtest)
#cxx_test(ord_tests gtest)

message( WARNING "Setting CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}" )

# --- Here's the existing CMake method

include_directories(/home/kuck/Downloads/googletest-release-1.8.0/googletest/include /home/kuck/Downloads/googletest-release-1.8.0/googlemock/include)
link_directories(/home/kuck/Downloads/googletest-release-1.8.0/googletest/build-clau /home/kuck/Downloads/googletest-release-1.8.0/googlemock/build-clau)

add_executable(GTestExperiments_T ord_tests.cpp)
target_link_libraries(GTestExperiments_T pthread gmock_main gmock gtest_main gtest)
add_test(GTestExperimentalTests GTestExperiments_T)

add_executable(OrdUnitTests_T OrdUnitTests_T.cpp)
target_link_libraries(OrdUnitTests_T gpstk pthread gmock_main gmock)
add_test(OrdUnitTests OrdUnitTests_T)

add_executable(OrdRegressionChecks_T OrdRegressionChecks_T.cpp)
target_link_libraries(OrdRegressionChecks_T gpstk pthread gmock_main gmock)
add_test(OrdRegressionCheck OrdRegressionChecks_T)