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
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_ANSI}
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
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_ANSI}
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
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_ANSI}
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
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_ANSI}
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
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_ANSI}
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
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_ANSI}
         -DTESTNAME=timeconvert_CmdOpt_GPSfullz
         -DTESTBASE=timeconvert_2015_200_99
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# check Julian date
set( TIMECONVERT_CMDOPT_JULIAN "-j 2457222.501146" )
add_test(NAME timeconvert_CmdOpt_Julian
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_ANSI}
         -DTESTNAME=timeconvert_CmdOpt_Julian
         -DTESTBASE=timeconvert_2015_200_99
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testopts.cmake)

# check MJD
set( TIMECONVERT_CMDOPT_MJD "-m 57222.001145833" )
add_test(NAME timeconvert_CmdOpt_MJD
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:timeconvert>
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_ANSI}
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
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_ANSI}
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
         -DPROG_OPTS=${TIMECONVERT_CMDOPT_ANSI}
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
