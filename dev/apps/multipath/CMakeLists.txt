# apps/multipath/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/rxio
${CMAKE_SOURCE_DIR}/lib/geomatics
${CMAKE_SOURCE_DIR}/lib/vdraw
${CMAKE_SOURCE_DIR}/lib/vplot
)

add_executable(mpsolve mpsolve.cpp ObsArray.cpp)
target_link_libraries(mpsolve rxio geomatics vplot)
install (TARGETS mpsolve DESTINATION bin)
