# apps/geomatics/robust/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/geomatics
)

add_executable(lsfilt lsfilt.cpp)
target_link_libraries(lsfilt gpstk geomatics)

add_executable(rstats rstats.cpp)
target_link_libraries(rstats gpstk geomatics)

