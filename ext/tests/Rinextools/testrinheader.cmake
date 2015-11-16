# test RinEdit header changes

execute_process(COMMAND ${TEST_PROG} --IF ${SOURCEDIR}/${INFILE} --OF ${TARGETDIR}/${TESTBASE}.out --HDr "RIGTC" --HDx '3979316.4389,1050312.2534,4857066.9036' --HDn 11502M002 --HDm GOPE --HDa "RIGTC, GO PECNY"
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed")
endif()


# Check that headers are in fact different
execute_process(COMMAND ${RINHEADDIFF} ${SOURCEDIR}/${INFILE} ${TARGETDIR}/${TESTBASE}.out
                RESULT_VARIABLE DIFFERENT)
if(DIFFERENT EQUAL 0)
    message(FATAL_ERROR "Test failed - expected non-zero result")
endif()


# Check that headers are the same when excluding expected changes
# Both of these sets of "set" directives seem to work under Solaris.
# Not sure if portable.
#set( EXCL1 PGM\ /\ RUN\ BY\ /\ DATE )
#set( EXCL2 APPROX\ POSITION\ XYZ )
#set( EXCL3 MARKER\ NUMBER )
#set( EXCL4 MARKER\ NAME )
#set( EXCL5 OBSERVER\ /\ AGENCY )
set( EXCL1 "PGM / RUN BY / DATE" )
set( EXCL2 "APPROX POSITION XYZ" )
set( EXCL3 "MARKER NUMBER" )
set( EXCL4 "MARKER NAME" )
set( EXCL5 "OBSERVER / AGENCY" )
execute_process(COMMAND ${RINHEADDIFF} -x ${EXCL1} -x ${EXCL2} -x ${EXCL3} -x ${EXCL4} -x ${EXCL5} ${SOURCEDIR}/${INFILE} ${TARGETDIR}/${TESTBASE}.out
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
