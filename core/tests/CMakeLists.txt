# tests/CMakeLists.txt

# A program to help app testing
add_executable(df_diff df_diff.cpp)
target_link_libraries(df_diff gpstk)
install (TARGETS df_diff DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_test(NAME df_diff_1
    COMMAND ${CMAKE_COMMAND}
    -DTEST_PROG=$<TARGET_FILE:df_diff>
    -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
    -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/testhelp.cmake)

# df_diff.in and arlm200b.15o differ in several ways
# a text difference in line 2 and 4 floating point
# differences of about 4e-11, 5e-7, 2e-5, and 4e-11

# the default should catch the 2e-5 and the text diff for 2 errors
add_test(NAME df_diff_2
    COMMAND ${CMAKE_COMMAND}
    -DTEST_PROG=$<TARGET_FILE:df_diff>
    -DTESTBASE=df_diff_2
    -DARGS=-1\ ${GPSTK_TEST_DATA_DIR}/arlm200b.15o\ -2\ ${GPSTK_TEST_DATA_DIR}/df_diff.in
    -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
    -DEXP_RC=2
    -DNODIFF=1
    -P ${CMAKE_CURRENT_SOURCE_DIR}/testsuccexp.cmake)

# ignore lines 1&2 and there should only be 1 error
add_test(NAME df_diff_3
    COMMAND ${CMAKE_COMMAND}
    -DTEST_PROG=$<TARGET_FILE:df_diff>
    -DTESTBASE=df_diff_3
    -DARGS=-1\ ${GPSTK_TEST_DATA_DIR}/arlm200b.15o\ -2\ ${GPSTK_TEST_DATA_DIR}/df_diff.in\ -l\ 2
    -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
    -DEXP_RC=1
    -DNODIFF=1
    -P ${CMAKE_CURRENT_SOURCE_DIR}/testsuccexp.cmake)

# ignore lines 1&2 and set eps to 1e-4 and there should be 0
add_test(NAME df_diff_4
    COMMAND ${CMAKE_COMMAND}
    -DTEST_PROG=$<TARGET_FILE:df_diff>
    -DTESTBASE=df_diff_3
    -DARGS=-1\ ${GPSTK_TEST_DATA_DIR}/arlm200b.15o\ -2\ ${GPSTK_TEST_DATA_DIR}/df_diff.in\ -l\ 2\ -e\ 1e-4
    -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
    -DEXP_RC=0
    -DNODIFF=1
    -P ${CMAKE_CURRENT_SOURCE_DIR}/testsuccexp.cmake)

# ignore lines 1&2, and the last 2617, set eps to 1e-14 and there should be 2
add_test(NAME df_diff_5
    COMMAND ${CMAKE_COMMAND}
    -DTEST_PROG=$<TARGET_FILE:df_diff>
    -DTESTBASE=df_diff_5
    -DARGS=-1\ ${GPSTK_TEST_DATA_DIR}/arlm200b.15o\ -2\ ${GPSTK_TEST_DATA_DIR}/df_diff.in\ -l\ 2\ -e\ 1e-14\ -z\ 2617
    -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
    -DEXP_RC=2
    -DNODIFF=1
    -P ${CMAKE_CURRENT_SOURCE_DIR}/testsuccexp.cmake)


# This function automates some of the setup of testing an application that generates
# a single output file and differences that against a reference file
# See Rinextools RinEdit for examples of how to use it.
function(test_app_with_file test_name app_name labels args diff_args)
    add_test(NAME ${test_name}
        COMMAND ${CMAKE_COMMAND}
        -DTEST_PROG=$<TARGET_FILE:${app_name}>
        -DTESTBASE=${test_name}
        -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}/outputs
        -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
        -DARGS=${args}
        -DOWNOUTPUT=1
        -DDIFF_PROG=$<TARGET_FILE:df_diff>
        -DDIFF_ARGS=${diff_args}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
    set_property(TEST ${test_name} PROPERTY LABELS ${labels})
endfunction()


# This function automates some of the setup of testing an application that generates
# output to standard output  and differences that against a reference file
# See Rinextools RinSum for examples of how to use it.
function(test_app_with_stdout test_name app_name labels args diff_args)
    add_test(NAME ${test_name}
        COMMAND ${CMAKE_COMMAND}
        -DTEST_PROG=$<TARGET_FILE:${app_name}>
        -DTESTBASE=${test_name}
        -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}/outputs
        -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
        -DARGS=${args}
        -DDIFF_PROG=$<TARGET_FILE:df_diff>
        -DDIFF_ARGS=${diff_args}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
    set_property(TEST ${test_name} PROPERTY LABELS ${labels})
endfunction()

if (TEST_SWITCH)
    # library testing
    add_subdirectory( ClockModel )
    add_subdirectory( TimeHandling )
    add_subdirectory( FileDirProc )
    add_subdirectory( Math )
    add_subdirectory( GNSSCore )
    add_subdirectory( FileHandling )
    add_subdirectory( Utilities )
    add_subdirectory( GNSSEph )
    add_subdirectory( RefTime )
    add_subdirectory( CommandLine )
    add_subdirectory( NavFilter )
    add_subdirectory( ORD )

    # application testing
    add_subdirectory( difftools )
    add_subdirectory( filetools )
    add_subdirectory( time )
    add_subdirectory( positioning )
    add_subdirectory( Rinextools )
    add_subdirectory( mergetools )
    add_subdirectory( checktools )
    add_subdirectory( AppFrame )
endif()
