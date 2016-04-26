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

add_test(NAME df_diff_2
    COMMAND ${CMAKE_COMMAND}
    -DTEST_PROG=$<TARGET_FILE:df_diff>
    -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
    -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/df_diff.ctest)

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
    add_subdirectory (NavFilter)
    
    # application testing
    add_subdirectory( time )
    add_subdirectory (positioning)
    add_subdirectory( Rinextools )
    
    # add_subdirectory( AppFrame )
endif()
