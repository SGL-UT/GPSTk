
add_executable(SEM_T SEM_T.cpp)
target_link_libraries(SEM_T gpstk)
add_test(NAME SEMData
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:SEM_T>
         -DTESTBASE=test_output_SEM_T
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../GNSSEph/testsame.cmake)
