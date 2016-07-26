###############################################################################
# Tests for mpsolve Multipath Environment Evaluator
#
# Expected variables (required unless otherwise noted):
# TEST_PROG: the program under test
# SOURCEDIR: the location of the input and reference files
# TARGETDIR: the directory in which to store output
# TESTBASE: the name of the test, used to find output and reference files
# IN_OBS_FILE1: first input file name (observation data)
# IN_NAV_FILE1: second input file name (ephemeris data)
#
# TEST_PROG is expected to generate output files such as:
#     ${TARGETDIR}/${TESTBASE}_foo.out
#
# Reference files are of the form:
#     ${SOURCEDIR}/${TESTBASE}_foo.exp
#
###############################################################################

message(STATUS "Running ${TEST_PROG} IN_OBS_FILE1=${SOURCEDIR}/${IN_OBS_FILE1} IN_NAV_FILE1=${SOURCEDIR}/${IN_NAV_FILE1}")


###############################################################################
# Ensure required options -o and -e are present and their arguments valid
###############################################################################

message(STATUS "mpsolve -o OK")
execute_process(COMMAND ${TEST_PROG} -o ${IN_OBS_FILE1}
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from missing required option -e")
endif()

message(STATUS "mpsolve -e OK")
execute_process(COMMAND ${TEST_PROG} -e ${IN_NAV_FILE1}
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from missing required option -o")
endif()

message(STATUS "mpsolve -o BAD -e OK")
execute_process(COMMAND ${TEST_PROG} -o missing_obs_file -e ${SOURCEDIR}/${IN_NAV_FILE1}
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_oe.out
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid observation file")
endif()

message(STATUS "mpsolve -o OK -e BAD")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e missing_nav_file
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_oe.out
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid navigation file")
endif()

message(STATUS "mpsolve -o OK -e OK")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1}
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_oe.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_oe.out ${SOURCEDIR}/test_output_mpsolve_oe.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()


###############################################################################
# Ensure proper handing of options -n/--numeric
###############################################################################

message(STATUS "mpsolve ... -n")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -n
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_oen.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_oen.out ${SOURCEDIR}/test_output_mpsolve_oen.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()

message(STATUS "mpsolve ... --numeric")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} --numeric
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_oen.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_oen.out ${SOURCEDIR}/test_output_mpsolve_oen.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()


###############################################################################
# Ensure proper handing of options -r/--raw
###############################################################################

message(STATUS "mpsolve ... -r")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -r
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_oer.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_oer.out ${SOURCEDIR}/test_output_mpsolve_oer.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()

message(STATUS "mpsolve ... --raw")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} --raw
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_oer.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_oer.out ${SOURCEDIR}/test_output_mpsolve_oer.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()


###############################################################################
# Ensure proper handing of options -a/--azimuth
###############################################################################

message(STATUS "mpsolve ... -a")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -a
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_oea.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_oea.out ${SOURCEDIR}/test_output_mpsolve_oea.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()

message(STATUS "mpsolve ... --azimuth")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} --azimuth
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_oea.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_oea.out ${SOURCEDIR}/test_output_mpsolve_oea.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()


###############################################################################
# Ensure proper handing of options -c/--complete
###############################################################################

message(STATUS "mpsolve ... -c")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -c
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_oec.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_oec.out ${SOURCEDIR}/test_output_mpsolve_oec.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()

message(STATUS "mpsolve ... --complete")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} --complete
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_oec.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_oec.out ${SOURCEDIR}/test_output_mpsolve_oec.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()


###############################################################################
# Ensure proper handing of options -d/--dfm
###############################################################################

message(STATUS "mpsolve ... -d")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -d
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_oed.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_oed.out ${SOURCEDIR}/test_output_mpsolve_oed.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()

message(STATUS "mpsolve ... --dfm")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} --dfm
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_oed.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_oed.out ${SOURCEDIR}/test_output_mpsolve_oed.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()


###############################################################################
# Ensure proper handing of options -n, -r, -a, -c, and -d together
###############################################################################

message(STATUS "mpsolve ... -nracd")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -nracd
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_oenracd.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_oenracd.out ${SOURCEDIR}/test_output_mpsolve_oenracd.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()


###############################################################################
# Ensure proper handling of options -b/--bin=ARG
###############################################################################

# Syntax error
message(STATUS "mpsolve ... -b 10:90")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -b 10:90
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid syntax")
endif()

# Missing bound
message(STATUS "mpsolve ... -b 20")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -b 20
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from missing bin boundary")
endif()

# Negative bound
message(STATUS "mpsolve ... -b -10,10")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -b -10,10
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_b_neg.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

# Backwards bounds
message(STATUS "mpsolve ... -b 40,10")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -b 40,10
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from backwards bin boundary")
endif()

# Single valid bin
message(STATUS "mpsolve ... -b 10,40")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -b 10,40
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_b.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

# Zero width bin
message(STATUS "mpsolve ... -b 30,30")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -b 30,30
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_b.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

# Multiple valid, adjacent bins
message(STATUS "mpsolve ... -b 10,40 -b 40,70")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -b 10,40 -b 40,70
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_bb.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

# Multiple valid, non-adjacent bins
message(STATUS "mpsolve ... -b 10,30 -b 40,60 -b 70,90")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -b 10,30 -b 40,60 -b 70,90
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_b_b_b.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

# Identical bounds
message(STATUS "mpsolve ... --bin=10,30 --bin=10,30")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} --bin=10,30 --bin=10,30
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_b2.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

# Overlapping bins
message(STATUS "mpsolve ... -b 10,50 -b 30,70 -b 50,90")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -b 10,50 -b 30,70 -b 50,90
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_bbb.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()


###############################################################################
# Ensure proper handling of options -m/--multipath=ARG
###############################################################################

# Invalid syntax
message(STATUS "mpsolve ... -m 'foo'")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -m foo
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid syntax")
endif()

# Invalid variable (J)
message(STATUS "mpsolve ... -m 'A+B'")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -m A+J
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid variable")
endif()

# Unsupported operation (e.g. trunc)
message(STATUS "mpsolve ... -m 'trunc(L1+L2)'")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -m trunc(L1+L2)
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid operation")
endif()

# Invalid operation (e.g. %)
message(STATUS "mpsolve ... -m 'L1%L2'")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -m L1%L2
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid operation")
endif()

# Valid expression (default)
message(STATUS "mpsolve ... -m 'P1-wl1*L1+2/(1-gamma)*(wl1*L1-wl2*L2)'")
set(EQARG "P1-wl1*L1+2/(1-gamma)*(wl1*L1-wl2*L2")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -m ${EQARG}
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_mOK.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_mOK.out ${SOURCEDIR}/test_output_mpsolve_mOK.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()

# Valid expression yielding crazy results
message(STATUS "mpsolve ... -m 'P1*wl1*L1+2/(1-gamma)*(wl1*L1-wl2*L2)'")
set(EQARG "P1*wl1*L1+2/(1-gamma)*(wl1*L1-wl2*L2)")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} --multipath=${EQARG}
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_mBAD.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_mBAD.out ${SOURCEDIR}/test_output_mpsolve_mBAD.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()

# Multiple valid options
message(STATUS "mpsolve ... -m -m 'L1*L2' -m 'L1+L2'")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -m L1*L2 -m L1+L2
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid variable")
endif()


###############################################################################
# Ensure proper handling of options -u/--upper=ARG
###############################################################################

message(STATUS "mpsolve ... -u -10")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -u -10
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid width (negative)")
endif()

message(STATUS "mpsolve ... -u 0")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -u 0
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_u0.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_u0.out ${SOURCEDIR}/test_output_mpsolve_u0.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()

message(STATUS "mpsolve ... -u 0 -u 5")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -u 0 -u 5
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from multiple upper options")
endif()

message(STATUS "mpsolve ... -u 10.0")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -u 10.0
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_u10.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

# -u 10 output should match -u 0
execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_u10.out ${SOURCEDIR}/test_output_mpsolve_u0.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()

message(STATUS "mpsolve ... -upper=30 -an")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} --upper=30 -an
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_u30an.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_u30an.out ${SOURCEDIR}/test_output_mpsolve_u30an.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()

message(STATUS "mpsolve ... -u 91")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -u 91
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid upper (overflow)")
endif()


###############################################################################
# Ensure proper handling of options -f/--file=ARG
###############################################################################
# Syntax error
message(STATUS "mpsolve ... -f 1:2016")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -f 1:2016
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid file option syntax")
endif()

# Syntax error
message(STATUS "mpsolve ... -f 1,2016:5,2016")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -f 1,2016:5,2016
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid file option syntax")
endif()

# Negative day
message(STATUS "mpsolve ... -f -1,2016,5,2016")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -f -1,2016,5,2016
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid file option value (negative day)")
endif()

# Out-of-bounds day (<1, >365)
message(STATUS "mpsolve ... -f 0,2016,5,2016")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -f 0,2016,5,2016
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid file option value (out-of-bounds day)")
endif()

# Out-of-bounds day (<1, >365)
message(STATUS "mpsolve ... -f 360,2015,370,2015")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -f 360,2015,370,2015
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid file option value (out-of-bounds day)")
endif()

# Negative year
message(STATUS "mpsolve ... -f 1,-1,5,5")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -f 1,-1,5,5
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid file option value (negative year)")
endif()

# Missing start/end
message(STATUS "mpsolve ... -f 1,2016")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -f 1,2016
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid file option value (missing end time)")
endif()

# Backwards bounds
message(STATUS "mpsolve ... -f 5,2016,1,2016")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -f 5,2016,1,2016
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid file option value (backwards bounds)")
endif()

# Identical bounds
message(STATUS "mpsolve ... -f 5,2016,5,2016")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -f 5,2016,5,2016
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_f00520160052016.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

# @TODO - Validate output

# Valid bounds
message(STATUS "mpsolve ... --file=1,2016,5,2016")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} --file=1,2016,5,2016
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_f00120160052016.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

# @TODO - Validate output

# Multiple valid options
message(STATUS "mpsolve ... -f 1,2016,5,2016 -f 7,2016,9,2016")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -f 1,2016,5,2016 -f 7,2016,9,2016
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error form multiple file options")
endif()


###############################################################################
# Ensure proper handling of options -l/--length=NUM
###############################################################################

message(STATUS "mpsolve ... -l -30")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -l -30
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid width (negative)")
endif()

message(STATUS "mpsolve ... -l 0")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -l 0
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_l0.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_l0.out ${SOURCEDIR}/test_output_mpsolve_l0.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()

message(STATUS "mpsolve ... -l 0 -l 5")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -l 0 -l 5
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from multiple length options")
endif()

message(STATUS "mpsolve ... -l 10.0")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -l 10.0
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid width (non-integral)")
endif()

message(STATUS "mpsolve ... -l 30")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -l 30
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_l30.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

# -l 30 output should match -l 0
execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_l30.out ${SOURCEDIR}/test_output_mpsolve_l0.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()

message(STATUS "mpsolve ... -l 900")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -l 900
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_l900.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

# -l 900 output should match -l 0
execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_l900.out ${SOURCEDIR}/test_output_mpsolve_l0.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()

# Exclude some satellite passes
message(STATUS "mpsolve ... --length=3000 -an")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} --length=3000 -an
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_l3000an.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_l3000an.out ${SOURCEDIR}/test_output_mpsolve_l3000an.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()

# Exclude all satellite passes
message(STATUS "mpsolve ... --length=3600 -an")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} --length=3600 -an
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_l3600an.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_l3600an.out ${SOURCEDIR}/test_output_mpsolve_l3600an.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()


###############################################################################
# Ensure proper handling of options -w/--width=NUM
###############################################################################

message(STATUS "mpsolve ... -w -15")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -w -15
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid width (negative)")
endif()

message(STATUS "mpsolve ... -w 0")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -w 0
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from invalid width (zero)")
endif()

message(STATUS "mpsolve ... -w -10.0")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -w 10.0
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(!HAD_ERROR)
   message(FATAL_ERROR "Test failed - expected error from invalid width (non-integral)")
endif()

message(STATUS "mpsolve ... -w -10")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -w 10
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_w10.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_w10.out ${SOURCEDIR}/test_output_mpsolve_w10.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()

message(STATUS "mpsolve ... -w 10 -w 15")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -w 10 -w 15
                OUTPUT_QUIET
                ERROR_QUIET
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
   message(FATAL_ERROR "Test failed - expected error from multiple width options")
endif()

message(STATUS "mpsolve ... -w 17")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} -w 17
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_w17.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_w17.out ${SOURCEDIR}/test_output_mpsolve_w17.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()

message(STATUS "mpsolve ... --width=30 -an")
execute_process(COMMAND ${TEST_PROG} -o ${SOURCEDIR}/${IN_OBS_FILE1} -e ${SOURCEDIR}/${IN_NAV_FILE1} --width=30 -an
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}_w30an.out
                TIMEOUT 15
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
   message(FATAL_ERROR "Test failed - an error code was returned")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TARGETDIR}/${TESTBASE}_w30an.out ${SOURCEDIR}/test_output_mpsolve_w30an.exp
                RESULT_VARIABLE FILES_DIFFER)
if(FILES_DIFFER)
   message(FATAL_ERROR "Test failed - test output did not match expected output")
endif()
