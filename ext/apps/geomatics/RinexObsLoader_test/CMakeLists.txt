# apps/geomatics/RinexObsLoader_test/CMakeLists.txt

add_executable(Rinex3ObsLoader_T Rinex3ObsLoader_T.cpp)
target_link_libraries(Rinex3ObsLoader_T gpstk)
install (TARGETS Rinex3ObsLoader_T DESTINATION "${CMAKE_INSTALL_BINDIR}")


