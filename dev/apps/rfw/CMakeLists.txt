# apps/rfw/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/rxio
)

add_executable(rfw rfw.cpp FDStreamBuff.cpp TCPStreamBuff.cpp)
target_link_libraries(rfw rxio)
install (TARGETS rfw DESTINATION bin)
