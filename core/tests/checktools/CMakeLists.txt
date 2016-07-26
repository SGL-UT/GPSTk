set( SD ${GPSTK_TEST_DATA_DIR} )
set( TD ${GPSTK_TEST_OUTPUT_DIR} )

###############################################################################
# TEST rmwcheck
###############################################################################

# check that -h option is valid
add_test(NAME rmwcheck_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwcheck>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -DGPSTK_BINDIR=${GPSTK_BINDIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testhelp.cmake)

# check a valid RINEX Met file
add_test(NAME rmwcheck_Valid
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwcheck>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -DNODIFF=1
         -DARGS=${SD}/arlm200a.15m
         -DGPSTK_BINDIR=${GPSTK_BINDIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# check a valid RINEX Nav file (should fail, as it isn't a Met file)
add_test(NAME rmwcheck_Invalid_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwcheck>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -DNODIFF=1
         -DARGS=${SD}/arlm200a.15n
         -DGPSTK_BINDIR=${GPSTK_BINDIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

# check a file that is not a RINEX file
add_test(NAME rmwcheck_Invalid_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwcheck>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -DNODIFF=1
         -DARGS=${SD}/timeconvert_2015_200_0.exp
         -DGPSTK_BINDIR=${GPSTK_BINDIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

# check a file that does not exist
#add_test(NAME rmwcheck_Missing
#         COMMAND ${CMAKE_COMMAND}
#         -DTEST_PROG=$<TARGET_FILE:rmwcheck>
#         -DSOURCEDIR=${SD}
#         -DTARGETDIR=${TD}
#         -DNODIFF=1
#         -DARGS=${SD}/notAFile.txt
#         -DGPSTK_BINDIR=${GPSTK_BINDIR}
#         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)


###############################################################################
# TEST rnwcheck
###############################################################################

# check that -h option is valid
add_test(NAME rnwcheck_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rnwcheck>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -DGPSTK_BINDIR=${GPSTK_BINDIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testhelp.cmake)

# check a valid RINEX Nav file
add_test(NAME rnwcheck_Valid
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rnwcheck>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -DNODIFF=1
         -DARGS=${SD}/arlm200a.15n
         -DGPSTK_BINDIR=${GPSTK_BINDIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# check a valid RINEX Obs file (should fail, as it isn't a Nav file)
add_test(NAME rnwcheck_Invalid_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rnwcheck>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -DNODIFF=1
         -DARGS=${SD}/arlm200a.15o
         -DGPSTK_BINDIR=${GPSTK_BINDIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

# check a file that is not a RINEX file
add_test(NAME rnwcheck_Invalid_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rnwcheck>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -DNODIFF=1
         -DARGS=${SD}/timeconvert_2015_200_0.exp
         -DGPSTK_BINDIR=${GPSTK_BINDIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

# check a file that does not exist
#add_test(NAME rnwcheck_Missing
#         COMMAND ${CMAKE_COMMAND}
#         -DTEST_PROG=$<TARGET_FILE:rnwcheck>
#         -DSOURCEDIR=${SD}
#         -DTARGETDIR=${TD}
#         -DNODIFF=1
#         -DARGS=${SD}/notAFile.txt
#         -DGPSTK_BINDIR=${GPSTK_BINDIR}
#         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

###############################################################################
# TEST rowcheck
###############################################################################

# check that -h option is valid
add_test(NAME rowcheck_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowcheck>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -DGPSTK_BINDIR=${GPSTK_BINDIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testhelp.cmake)

# check a valid RINEX Obs file
add_test(NAME rowcheck_Valid
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowcheck>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -DNODIFF=1
         -DARGS=${SD}/arlm200a.15o
         -DGPSTK_BINDIR=${GPSTK_BINDIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# check a valid RINEX Met file (should fail, as it isn't a Obs file)
add_test(NAME rowcheck_Invalid_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowcheck>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -DNODIFF=1
         -DARGS=${SD}/arlm200a.15m
         -DGPSTK_BINDIR=${GPSTK_BINDIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

# check a file that is not a RINEX file
add_test(NAME rowcheck_Invalid_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowcheck>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -DNODIFF=1
         -DARGS=${SD}/timeconvert_2015_200_0.exp
         -DGPSTK_BINDIR=${GPSTK_BINDIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

# check a file that does not exist
#add_test(NAME rowcheck_Missing
#         COMMAND ${CMAKE_COMMAND}
#         -DTEST_PROG=$<TARGET_FILE:rowcheck>
#         -DSOURCEDIR=${SD}
#         -DTARGETDIR=${TD}
#         -DNODIFF=1
#         -DARGS=${SD}/notAFile.txt
#         -DGPSTK_BINDIR=${GPSTK_BINDIR}
#         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)
