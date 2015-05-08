# apps/time/CMakeLists.txt

add_executable(timeconvert timeconvert.cpp)
target_link_libraries(timeconvert gpstk)
install (TARGETS timeconvert DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(calgps calgps.cpp)
target_link_libraries(calgps gpstk)
install (TARGETS calgps DESTINATION "${CMAKE_INSTALL_BINDIR}")

