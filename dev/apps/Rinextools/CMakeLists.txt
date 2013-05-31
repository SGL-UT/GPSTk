# apps/Rinextools/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
)

add_executable(RinDump RinDump.cpp)
target_link_libraries(RinDump gpstk rxio)

add_executable(RinEdit RinEdit.cpp)
target_link_libraries(RinEdit gpstk rxio)

add_executable(RinNav RinNav.cpp)
target_link_libraries(RinNav gpstk rxio)

add_executable(RinSum RinSum.cpp)
target_link_libraries(RinSum gpstk rxio)

