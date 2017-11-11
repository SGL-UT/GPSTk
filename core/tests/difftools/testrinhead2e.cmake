# tests for rinheaddiff

message(STATUS "running ${TEST_PROG} -x ANT\\ #\\ /\\ TYPE -x ANTENNA:\\ DELTA\\ H/E/N -x APPROX\\ POSITION\\ XYZ -x COMMENT -x MARKER\\ NAME -x MARKER\\ NUMBER -x MARKER\\ TYPE -x OBSERVER\\ /\\ AGENCY -x PGM\\ /\\ RUN\\ BY\\ /\\ DATE -x REC\\ #\\ /\\ TYPE\\ /\\ VERS ${SOURCEDIR}/${FILE1} ${SOURCEDIR}/${FILE2}" )

execute_process(COMMAND ${TEST_PROG} -x "ANT # / TYPE" -x "ANTENNA: DELTA H/E/N" -x "APPROX POSITION XYZ" -x "COMMENT" -x "MARKER NAME" -x "MARKER NUMBER" -x "MARKER TYPE" -x "OBSERVER / AGENCY" -x "PGM / RUN BY / DATE" -x "REC # / TYPE / VERS" ${SOURCEDIR}/${FILE1} ${SOURCEDIR}/${FILE2}
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}.out
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed: ${HAD_ERROR}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files
    ${SOURCEDIR}/${TESTBASE}.exp ${TARGETDIR}/${TESTBASE}.out
    RESULT_VARIABLE DIFFERENT)
if(DIFFERENT)
    message(FATAL_ERROR "Test failed - files differ")
endif()
