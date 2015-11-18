# tests/CMakeLists.txt

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

# application testing
add_subdirectory( time )
add_subdirectory (positioning)

# add_subdirectory( AppFrame )

