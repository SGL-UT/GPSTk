# test that files are the same

# Convert ARGS into a cmake list
if (ARGS)
   string(REPLACE " " ";" ARG_LIST ${ARGS})
else()
   set(ARG_LIST "")
endif()

message(STATUS "running ${TEST_PROG} --in ${SOURCEDIR}/${FILE1} --in ${SOURCEDIR}/${FILE2} --out ${TARGETDIR}/${TESTBASE}.out ${ARGS} ")

execute_process(COMMAND ${TEST_PROG} --in ${SOURCEDIR}/${FILE1} --in ${SOURCEDIR}/${FILE2} ${ARG_LIST} --out ${TARGETDIR}/${TESTBASE}.out
                RESULT_VARIABLE HAD_ERROR)

# files are expected to be different
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files
    ${SOURCEDIR}/${TESTBASE}.exp ${TARGETDIR}/${TESTBASE}.out
    RESULT_VARIABLE DIFFERENT)
if(DIFFERENT)
    message(FATAL_ERROR "Test failed - files differ")
endif()
