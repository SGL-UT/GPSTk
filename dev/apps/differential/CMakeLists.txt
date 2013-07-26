# apps/differential/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
)

add_executable(vecsol vecsol.cpp)
target_link_libraries(vecsol gpstk)
install (TARGETS vecsol DESTINATION bin)
