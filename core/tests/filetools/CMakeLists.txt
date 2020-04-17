set( SD ${GPSTK_TEST_DATA_DIR} )
set( TD ${GPSTK_TEST_OUTPUT_DIR} )

###############################################################################
# TEST bc2sp3
###############################################################################

# check that -h option is valid
add_test(NAME bc2sp3_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:bc2sp3>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testhelp_bc.cmake)

# check that bc2sp3 fails with missing files
#add_test(NAME bc2sp3_CmdOpt_2
#         COMMAND ${CMAKE_COMMAND}
#         -DTEST_PROG=$<TARGET_FILE:bc2sp3>
#         -DSOURCEDIR=${SD}
#         -DTARGETDIR=${TD}
#         -P ${CMAKE_CURRENT_SOURCE_DIR}/testmissing.cmake)

# check that bc2sp3 fails with invalid input
#add_test(NAME bc2sp3_InvalidInput
#         COMMAND ${CMAKE_COMMAND}
#         -DTEST_PROG=$<TARGET_FILE:bc2sp3>
#         -DARGS=--in\ ${SD}/timeconvert_2015_200_0.exp\ --out\ ${TD}/bc2sp3_InvalidInput.out
#         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

# check the output against prepared "truth" - SP3c case
set(ARGS_1 "--outputC --tb 1825,518400 --te 1825,604500")
add_test(NAME bc2sp3_Same_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:bc2sp3>
         -DFILE1=nga002.15n
         -DFILE2=nga003.15n
         -DTESTBASE=bc2sp3_Same_1
         -DTESTNAME=bc2sp3_Same_1
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -DARGS=${ARGS_1}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsame.cmake)

# check the output against prepared "truth" - SP3a case
set(ARGS_2 "--tb 1825,518400 --te 1825,604500 --msg Test_Case_File" )
add_test(NAME bc2sp3_Same_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:bc2sp3>
         -DFILE1=nga002.15n
         -DFILE2=nga003.15n
         -DTESTBASE=bc2sp3_Same_2
         -DTESTNAME=bc2sp3_Same_2
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -DARGS=${ARGS_2}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsame.cmake)

# check the output against prepared "truth" - SP3a case, 900s cadence
set(ARGS_2 "--tb 1825,518400 --te 1825,603900 --cs 900" )
add_test(NAME bc2sp3_Same_3
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:bc2sp3>
         -DFILE1=nga002.15n
         -DFILE2=nga003.15n
         -DTESTBASE=bc2sp3_Same_3
         -DTESTNAME=bc2sp3_Same_3
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -DARGS=${ARGS_2}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsame.cmake)


# tests for smdscheck

add_test(NAME smdscheck_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:smdscheck>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testhelp.cmake)

set(SMDSCHECK_VALID_ARGS "-M ${SD}/sats.csv")
add_test(NAME smdscheck_valid
  COMMAND ${CMAKE_COMMAND}
  -DTEST_PROG=$<TARGET_FILE:smdscheck>
  -DSOURCEDIR=${SD}
  -DTARGETDIR=${TD}
  -DTESTNAME=smdscheck_valid
  -DTESTBASE=smdscheck_valid
  -DARGS=${SMDSCHECK_VALID_ARGS}
  -DNODIFF=1
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

set(SMDSCHECK_INVALID_ARGS "-M ${SD}/TropModel_Zero.exp")
add_test(NAME smdscheck_invalid
  COMMAND ${CMAKE_COMMAND}
  -DTEST_PROG=$<TARGET_FILE:smdscheck>
  -DARGS=${SMDSCHECK_INVALID_ARGS}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

set(SMDSCHECK_MISSING_ARGS "-M ${SD}/not_a_real_file.what")
add_test(NAME smdscheck_missing
  COMMAND ${CMAKE_COMMAND}
  -DTEST_PROG=$<TARGET_FILE:smdscheck>
  -DARGS=${SMDSCHECK_MISSING_ARGS}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)
