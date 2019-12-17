
set( TD ${GPSTK_TEST_OUTPUT_DIR} )
set( ED ${GPSTK_TEST_DATA_DIR}/expected)
set( df_diff ${GPSTK_BINDIR}/df_diff)

add_executable(Yuma_T Yuma_T.cpp)
target_link_libraries(Yuma_T gpstk)
add_test(GNSSEph_Yuma Yuma_T)
#set_property(TEST GNSSEph_Yuma PROPERTY LABELS GNSSEph Yuma)

execute_process(COMMAND ${df_diff} -v -1 ${ED}/test_output_Yuma_T.exp -2 
   ${TD}/test_output_Yuma_T.out 
   RESULT_VARIABLE DIFFERENT)
if(DIFFERENT)
#   message(FATAL_ERROR "Test failed - files differ")
endif()