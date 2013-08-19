# apps/geomatics/kalman/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/geomatics
)

add_executable(mergeSRI mergeSRI.cpp)
target_link_libraries(mergeSRI geomatics)
install (TARGETS mergeSRI DESTINATION bin)

add_executable(tkalm tkalm.cpp)
target_link_libraries(tkalm geomatics)
install (TARGETS tkalm DESTINATION bin)
