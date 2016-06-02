# test RinSum

message(STATUS "running ${TEST_PROG} --obs ${SOURCEDIR}/${INFILE} ${OPTS} --quiet")
execute_process(COMMAND ${TEST_PROG} --obs ${SOURCEDIR}/${INFILE} ${OPTS} --quiet
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}.out
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed")
endif()


execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files
    ${SOURCEDIR}/${TESTBASE}.exp ${TARGETDIR}/${TESTBASE}.out
    RESULT_VARIABLE DIFFERENT)
if(DIFFERENT)
    message(FATAL_ERROR "Test failed - files differ")
endif()
