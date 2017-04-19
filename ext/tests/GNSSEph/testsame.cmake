# test that file produced matches expectations.

message(STATUS "running ${TEST_PROG} ${ARGS}")

execute_process(COMMAND ${TEST_PROG} ${ARGS}
                RESULT_VARIABLE HAD_ERROR)

if(HAD_ERROR)
    message(FATAL_ERROR "Test failed")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files
    ${SOURCEDIR}/${TESTBASE}.exp ${TARGETDIR}/${TESTBASE}.out
    RESULT_VARIABLE DIFFERENT)
if(DIFFERENT)
    message(FATAL_ERROR "Test failed - files differ")
else()
    message(STATUS "test file compare passed")
endif(DIFFERENT)
