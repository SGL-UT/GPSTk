#Tests for AppFrame Classes

#add_executable(HelmertTransform_T HelmertTransform_T.cpp ftest.cpp)
#target_link_libraries(HelmertTransform_T gpstk)
#add_test(RefTime_HelmertTransform HelmertTransform_T)

add_executable(BasicFrameworkHelp_T BasicFrameworkHelp_T.cpp)
target_link_libraries(BasicFrameworkHelp_T gpstk)

add_executable(BasicFrameworkHelpReq_T BasicFrameworkHelp_T.cpp)
target_link_libraries(BasicFrameworkHelpReq_T gpstk)

# test behavior with command-line option usage
add_test(NAME BasicFrameworkHelp_T_1
  COMMAND ${CMAKE_COMMAND}
  -DTEST_PROG=$<TARGET_FILE:BasicFrameworkHelp_T>
  -DTESTBASE=BasicFrameworkHelp_T_1
  -DDIFFSTDERR=yes
  -DARGS=-h
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Same as above but using the long option instead. Output is the same,
# so use the same TESTBASE value
add_test(NAME BasicFrameworkHelp_T_2
  COMMAND ${CMAKE_COMMAND}
  -DTEST_PROG=$<TARGET_FILE:BasicFrameworkHelp_T>
  -DTESTBASE=BasicFrameworkHelp_T_1
  -DDIFFSTDERR=yes
  -DARGS=--help
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# test behavior with secondary help, no args
add_test(NAME BasicFrameworkHelp_T_3
  COMMAND ${CMAKE_COMMAND}
  -DTEST_PROG=$<TARGET_FILE:BasicFrameworkHelp_T>
  -DTESTBASE=BasicFrameworkHelp_T_3
  -DDIFFSTDERR=yes
  -DARGS=-x
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# test behavior with secondary help, with args
add_test(NAME BasicFrameworkHelp_T_4
  COMMAND ${CMAKE_COMMAND}
  -DTEST_PROG=$<TARGET_FILE:BasicFrameworkHelp_T>
  -DTESTBASE=BasicFrameworkHelp_T_4
  -DDIFFSTDERR=yes
  -DARGS=-y\ JunkText
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Multiple help-like options, expect the output to be the same as test 1
add_test(NAME BasicFrameworkHelp_T_5
  COMMAND ${CMAKE_COMMAND}
  -DTEST_PROG=$<TARGET_FILE:BasicFrameworkHelp_T>
  -DTESTBASE=BasicFrameworkHelp_T_1
  -DDIFFSTDERR=yes
  -DARGS=-h\ -x\ -y\ JunkText
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Multiple help-like options, expect the output to be the same as test 3
add_test(NAME BasicFrameworkHelp_T_6
  COMMAND ${CMAKE_COMMAND}
  -DTEST_PROG=$<TARGET_FILE:BasicFrameworkHelp_T>
  -DTESTBASE=BasicFrameworkHelp_T_3
  -DDIFFSTDERR=yes
  -DARGS=-x\ -y\ JunkText\ -h
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Multiple help-like options, expect the output to be the same as test 4
add_test(NAME BasicFrameworkHelp_T_7
  COMMAND ${CMAKE_COMMAND}
  -DTEST_PROG=$<TARGET_FILE:BasicFrameworkHelp_T>
  -DTESTBASE=BasicFrameworkHelp_T_4
  -DDIFFSTDERR=yes
  -DARGS=-y\ JunkText\ -h\ -x
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Test simple help
add_test(NAME BasicFrameworkHelp_T_8
  COMMAND ${CMAKE_COMMAND}
  -DTEST_PROG=$<TARGET_FILE:BasicFrameworkHelp_T>
  -DTESTBASE=BasicFrameworkHelp_T_8
  -DDIFFSTDERR=yes
  -DARGS=-w
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Test behavior with required options, first with standard usage
# Won't be the same as non-req test #1 because the app name has changed.
add_test(NAME BasicFrameworkHelpReq_T_1
  COMMAND ${CMAKE_COMMAND}
  -DTEST_PROG=$<TARGET_FILE:BasicFrameworkHelpReq_T>
  -DTESTBASE=BasicFrameworkHelpReq_T_1
  -DDIFFSTDERR=yes
  -DARGS=-h
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Test behavior with required options, first with no-arg extra help
# Should be the same as without req option
add_test(NAME BasicFrameworkHelpReq_T_2
  COMMAND ${CMAKE_COMMAND}
  -DTEST_PROG=$<TARGET_FILE:BasicFrameworkHelpReq_T>
  -DTESTBASE=BasicFrameworkHelp_T_3
  -DDIFFSTDERR=yes
  -DARGS=-x
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Test behavior with required options, first with no-arg extra help
# Should be the same as without req option
add_test(NAME BasicFrameworkHelpReq_T_3
  COMMAND ${CMAKE_COMMAND}
  -DTEST_PROG=$<TARGET_FILE:BasicFrameworkHelpReq_T>
  -DTESTBASE=BasicFrameworkHelp_T_4
  -DDIFFSTDERR=yes
  -DARGS=-y\ JunkText
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
