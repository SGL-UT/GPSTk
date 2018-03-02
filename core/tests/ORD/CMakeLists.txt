
message( STATUS "Setting CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}" )

# --- Here's the existing CMake method

# /usr/lib64/libgmock.so
find_path(GMOCK_SOURCE_DIR NAMES src/gmock.cc CMakeLists.txt
          PATHS /usr/src/gmock /usr/local/src/gmock)
find_path(GMOCK_INCLUDE_DIR NAMES gmock.h
          PATHS /usr/include/gmock /usr/local/include/gmock)

# If we can't locate the INCLUDE directory, just give up.
if ( GMOCK_INCLUDE_DIR )
    # On Debian, gtest/gmock source is installed so it must be compiled.
    # On RedHat shared libraries are installed in standard locations.
    # MacOSX TBD
    # Windows TBD
    
    # Handle Debian-style installation
    if( GMOCK_SOURCE_DIR )
        message( STATUS "Setting GMOCK_SOURCE_DIR = ${GMOCK_SOURCE_DIR}" )
        # It appears that using gmock will create gtest automatically.
        # Including both will cause cmake to cry.
        #add_subdirectory(/usr/src/gtest ${CMAKE_CURRENT_BINARY_DIR}/gtest )
        add_subdirectory(${GMOCK_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/gmock )
    endif( )

    # STOPGAP until debian build machine has gmock installed.
    #include_directories(/home/kuck/Downloads/googletest-release-1.8.0/googletest/include /home/kuck/Downloads/googletest-release-1.8.0/googlemock/include)
    #link_directories(/home/kuck/Downloads/googletest-release-1.8.0/googletest/build-clau /home/kuck/Downloads/googletest-release-1.8.0/googlemock/build-clau)
    
    add_executable(GTestExperiments_T GTestExperiments.cpp)
    target_link_libraries(GTestExperiments_T pthread gmock_main gmock gtest_main gtest)
    add_test(GTestExperimentalTests GTestExperiments_T)
    
    add_executable(OrdUnitTests_T OrdUnitTests_T.cpp)
    target_link_libraries(OrdUnitTests_T gpstk pthread gmock_main)
    add_test(OrdUnitTests OrdUnitTests_T)
    
    add_executable(OrdRegressionChecks_T OrdRegressionChecks_T.cpp)
    target_link_libraries(OrdRegressionChecks_T gpstk pthread gmock_main gmock)
    add_test(OrdRegressionCheck OrdRegressionChecks_T)
else ( )
    message( WARNING "Unable to locate GTest/GMock.  Some tests cannot be run." )
endif( )

