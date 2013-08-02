# apps/time/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/vdraw
)

add_executable(timeconvert timeconvert.cpp)
target_link_libraries(timeconvert gpstk)
install (TARGETS timeconvert DESTINATION bin)

add_executable(calgps calgps.cpp)
target_link_libraries(calgps gpstk vdraw)
install (TARGETS calgps DESTINATION bin)

