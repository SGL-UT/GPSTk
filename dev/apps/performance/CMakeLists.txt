# apps/performance/CMakeLists.txt

add_executable(ConstellationList ConstellationList.cpp ConstellationSet.cpp ConstellationDefinition.cpp)
target_link_libraries(ConstellationList gpstk)
install (TARGETS ConstellationList DESTINATION bin)


