# test RinEdit header changes

execute_process(COMMAND ${TEST_PROG} --IF ${SOURCEDIR}/${INFILE} --OF ${TARGETDIR}/${TESTBASE}.out --HDr "RIGTC" --HDx '3979316.4389,1050312.2534,4857066.9036' --HDn 11502M002 --HDm GOPE --HDa "RIGTC, GO PECNY"
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed")
endif()


# Check that headers are in fact different
execute_process(COMMAND ${RINHEADDIFF} ${SOURCEDIR}/${INFILE} ${TARGETDIR}/${TESTBASE}.out
                RESULT_VARIABLE DIFFERENT)
if(!DIFFERENT)
    message(FATAL_ERROR "Test failed - expected non-zero result")
endif()


# Check that headers are the same when excluding expected changes
execute_process(COMMAND ${RINHEADDIFF} ${SOURCEDIR}/${INFILE} ${TARGETDIR}/${TESTBASE}.out -x \"PGM / RUN BY / DATE\" -x \"APPROX POSITION XYZ\" -x \"MARKER NUMBER\" -x \"MARKER NAME\" -x \"OBSERVER / AGENCY\"
                RESULT_VARIABLE DIFFERENT)
if(DIFFERENT)
    message(FATAL_ERROR "Test failed - unexpected header change")
endif()


# Check that data hasn't been mangled
execute_process(COMMAND ${ROWDIFF} ${SOURCEDIR}/${INFILE} ${TARGETDIR}/${TESTBASE}.out
                OUTPUT_QUIET
                RESULT_VARIABLE DIFFERENT)
if(DIFFERENT)
    message(FATAL_ERROR "Test failed - files differ")
endif()
