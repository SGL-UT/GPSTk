# apps/geomatics/JPLeph/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/geomatics
)

add_executable(convertSSEph convertSSEph.cpp)
target_link_libraries(convertSSEph geomatics)
install (TARGETS convertSSEph DESTINATION bin)

add_executable(testSSEph testSSEph.cpp)
target_link_libraries(testSSEph geomatics)
install (TARGETS testSSEph DESTINATION bin)

