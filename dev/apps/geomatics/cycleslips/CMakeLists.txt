# apps/geomatics/cycleslips/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/geomatics
)

add_executable(DiscFix DiscFix.cpp)
target_link_libraries(DiscFix geomatics)
install (TARGETS DiscFix DESTINATION bin)

