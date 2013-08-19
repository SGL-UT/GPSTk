# apps/Rinextools/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
)

add_executable(RinDump RinDump.cpp)
target_link_libraries(RinDump rxio)
install (TARGETS RinDump DESTINATION bin)

add_executable(RinEdit RinEdit.cpp)
target_link_libraries(RinEdit rxio)
install (TARGETS RinEdit DESTINATION bin)

add_executable(RinNav RinNav.cpp)
target_link_libraries(RinNav rxio)
install (TARGETS RinNav DESTINATION bin)

add_executable(RinSum RinSum.cpp)
target_link_libraries(RinSum rxio)
install (TARGETS RinSum DESTINATION bin)

