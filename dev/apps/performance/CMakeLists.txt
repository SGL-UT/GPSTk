# apps/performance/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
)

add_executable(ConstellationList ConstellationList.cpp ConstellationSet.cpp ConstellationDefinition.cpp)
target_link_libraries(ConstellationList gpstk)
install (TARGETS ConstellationList DESTINATION bin)


