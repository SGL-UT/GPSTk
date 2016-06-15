###############################################################################
# TEST timeconvert
###############################################################################

# These tests check both that options haven't changed and output
# format hasn't changed.

# check that -h option is valid
add_test(NAME timeconvert_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testhelp.cmake)

# check ANSI time
set( TIMECONVERT_CMDOPT_ANSI "-A 1437264099" )
add_test(NAME timeconvert_CmdOpt_ANSI
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_ANSI}
         -DTESTNAME=timeconvert_CmdOpt_ANSI
         -DTESTBASE=timeconvert_2015_200_99
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# check Civil time
set( TIMECONVERT_CMDOPT_CIVIL "-c 7 19 2015 00:01:39" )
add_test(NAME timeconvert_CmdOpt_Civil
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_CIVIL}
         -DTESTNAME=timeconvert_CmdOpt_Civil
         -DTESTBASE=timeconvert_2015_200_99
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# check RINEX time
set( TIMECONVERT_CMDOPT_RINEX "-R 15  7 19  0  1 39" )
add_test(NAME timeconvert_CmdOpt_Rinex
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_RINEX}
         -DTESTNAME=timeconvert_CmdOpt_Rinex
         -DTESTBASE=timeconvert_2015_200_99
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# check GPS epoch/week/second
set( TIMECONVERT_CMDOPT_GPSEWS "-o 1 830 99" )
add_test(NAME timeconvert_CmdOpt_GPSews
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_GPSEWS}
         -DTESTNAME=timeconvert_CmdOpt_GPSews
         -DTESTBASE=timeconvert_2015_200_99
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# check GPS full week/second
set( TIMECONVERT_CMDOPT_GPSWS "-f 1854 99" )
add_test(NAME timeconvert_CmdOpt_GPSws
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_GPSWS}
         -DTESTNAME=timeconvert_CmdOpt_GPSws
         -DTESTBASE=timeconvert_2015_200_99
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# check GPS full week/zcount
set( TIMECONVERT_CMDOPT_GPSWZ "-w 1854 66" )
add_test(NAME timeconvert_CmdOpt_GPSwz
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_GPSWZ}
         -DTESTNAME=timeconvert_CmdOpt_GPSwz
         -DTESTBASE=timeconvert_2015_200_99
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# check GPS 32-bit zcount
set( TIMECONVERT_CMDOPT_GPSFULLZ "-Z 972030018" )
add_test(NAME timeconvert_CmdOpt_GPSfullz
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_GPSFULLZ}
         -DTESTNAME=timeconvert_CmdOpt_GPSfullz
         -DTESTBASE=timeconvert_2015_200_99
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# check GPS 29-bit zcount
set( TIMECONVERT_CMDOPT_GPSZCOUNT29 "--z29 1 435159106" )
add_test(NAME timeconvert_CmdOpt_GPSzcount29
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_GPSZCOUNT29}
         -DTESTNAME=timeconvert_CmdOpt_GPSzcount29
         -DTESTBASE=timeconvert_2015_200_99
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# check Julian date
set( TIMECONVERT_CMDOPT_JULIAN "-j 2457222.50114583333" )
add_test(NAME timeconvert_CmdOpt_Julian
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_JULIAN}
         -DTESTNAME=timeconvert_CmdOpt_Julian
         -DTESTBASE=timeconvert_2015_200_99
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# check MJD
set( TIMECONVERT_CMDOPT_MJD "-m 57222.00114583333" )
add_test(NAME timeconvert_CmdOpt_MJD
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_MJD}
         -DTESTNAME=timeconvert_CmdOpt_MJD
         -DTESTBASE=timeconvert_2015_200_99
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# check Unix
set( TIMECONVERT_CMDOPT_UNIX "-u 1437264099 0" )
add_test(NAME timeconvert_CmdOpt_Unix
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_UNIX}
         -DTESTNAME=timeconvert_CmdOpt_Unix
         -DTESTBASE=timeconvert_2015_200_99
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# check YDS
set( TIMECONVERT_CMDOPT_YDS "-y 2015 200 99" )
add_test(NAME timeconvert_CmdOpt_YDS
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_YDS}
         -DTESTNAME=timeconvert_CmdOpt_YDS
         -DTESTBASE=timeconvert_2015_200_99
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# check subtract offset
set( TIMECONVERT_CMDOPT_SUBTRACT "-y 2015 200 99" )
set( TIMECONVERT_CMDOPT_SUBTRACT_2 "-s99" )
add_test(NAME timeconvert_CmdOpt_Subtract
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_SUBTRACT}
         -DPROG_OPTS_2=${TIMECONVERT_CMDOPT_SUBTRACT_2}
         -DTESTNAME=timeconvert_CmdOpt_Subtract
         -DTESTBASE=timeconvert_2015_200_0
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# check add offset
set( TIMECONVERT_CMDOPT_ADD "-y 2015 200 0" )
set( TIMECONVERT_CMDOPT_ADD_2 "-a99" )
add_test(NAME timeconvert_CmdOpt_Add
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_ADD}
         -DPROG_OPTS_2=${TIMECONVERT_CMDOPT_ADD_2}
         -DTESTNAME=timeconvert_CmdOpt_Add
         -DTESTBASE=timeconvert_2015_200_99
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# check output format
set( TIMECONVERT_CMDOPT_FORMAT "-y 2015 200 99" )
set( TIMECONVERT_CMDOPT_FORMAT_2 "-F%14.6J %15.9Q %4.1f %13.6g %12.6s %B %C %E %F %G %H %K %M %S %U %Y %Z %b %c %d %j %m %u %w %y %z" )
add_test(NAME timeconvert_CmdOpt_Format
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_FORMAT}
         -DPROG_OPTS_2=${TIMECONVERT_CMDOPT_FORMAT_2}
         -DTESTNAME=timeconvert_CmdOpt_Format
         -DTESTBASE=timeconvert_2015_200_format
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# MISSING:
# --input-format / --input-time
# --z29

###############################################################################
# TEST calgps
###############################################################################

# These tests check both that options haven't changed and output
# format hasn't changed.

# Check that -h option is valid.  calgps doesn't use BasicFramework so
# we don't do the usual checks of verbose and debug and so on.
add_test(NAME calgps_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_CmdOpt_1
         -DARGS=-h
         -DNODIFF=1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Check that -3 option is valid.
add_test(NAME calgps_CmdOpt_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_CmdOpt_2
         -DARGS=-3
         -DNODIFF=1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Check that -y option is valid.
add_test(NAME calgps_CmdOpt_3
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_CmdOpt_3
         -DARGS=-y
         -DNODIFF=1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Check that no options is valid.
add_test(NAME calgps_CmdOpt_4
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_CmdOpt_4
         -DNODIFF=1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Check that -Y produces expected output.
add_test(NAME calgps_Year
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_Year
         -DARGS=-Y\ 2015
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Check that -p produces expected output.
add_test(NAME calgps_Postscript
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_Postscript
         -DOWNOUTPUT=1
         -DARGS=-Y\ 2015\ -p\ ${GPSTK_TEST_OUTPUT_DIR}/calgps_Postscript.out
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Check that -n with -p produces expected output.
add_test(NAME calgps_Postscript_Quiet
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_Postscript_Quiet
         -DOWNOUTPUT=1
         -DARGS=-n\ -Y\ 2015\ -p\ ${GPSTK_TEST_OUTPUT_DIR}/calgps_Postscript_Quiet.out
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Check that -e produces expected output.
add_test(NAME calgps_EPS
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_EPS
         -DOWNOUTPUT=1
         -DARGS=-Y\ 2015\ -e\ ${GPSTK_TEST_OUTPUT_DIR}/calgps_EPS.out
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Check that -s produces expected output.
add_test(NAME calgps_SVG
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_SVG
         -DOWNOUTPUT=1
         -DARGS=-Y\ 2015\ -s\ ${GPSTK_TEST_OUTPUT_DIR}/calgps_SVG.out
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
