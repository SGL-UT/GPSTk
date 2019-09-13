
# --- Here's the existing CMake method

# /usr/lib64/libgmock.so
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

find_package(GMock REQUIRED)
include_directories(${GMOCK_INCLUDE_DIRS})

add_executable(GTestExperiments_T GTestExperiments.cpp)
target_link_libraries(GTestExperiments_T pthread ${GMOCK_BOTH_LIBRARIES} ${GTEST_BOTH_LIBRARIES})
add_test(GTestExperimentalTests GTestExperiments_T)

add_executable(OrdUnitTests_T OrdUnitTests_T.cpp)
target_link_libraries(OrdUnitTests_T gpstk pthread ${GMOCK_BOTH_LIBRARIES})
add_test(OrdUnitTests OrdUnitTests_T)

add_executable(OrdRegressionChecks_T OrdRegressionChecks_T.cpp)
target_link_libraries(OrdRegressionChecks_T gpstk pthread ${GMOCK_BOTH_LIBRARIES})
add_test(OrdRegressionCheck OrdRegressionChecks_T)


