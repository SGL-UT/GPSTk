
set( df_diff ${GPSTK_BINDIR}/df_diff)

add_executable(Yuma_T Yuma_T.cpp)
target_link_libraries(Yuma_T gpstk)
add_test(NAME YumaData
         COMMAND ${CMAKE_COMMAND}
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTNAME=test_output_Yuma_T
         -DTESTBASE=test_output_Yuma_T
         -DTEST_PROG=$<TARGET_FILE:Yuma_T>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DOWNOUTPUT=1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testsuccexp.cmake)
