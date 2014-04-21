# apps/time/CMakeLists.txt

add_executable(timeconvert timeconvert.cpp)
target_link_libraries(timeconvert gpstk)
install (TARGETS timeconvert DESTINATION bin)
add_test(timeconvertTest timeconvert)

add_executable(calgps calgps.cpp)
target_link_libraries(calgps gpstk)
install (TARGETS calgps DESTINATION bin)
add_test(calgpsTest calgps)

