# apps/rfw/CMakeLists.txt

add_executable(rfw rfw.cpp FDStreamBuff.cpp TCPStreamBuff.cpp)
target_link_libraries(rfw gpstk)
install (TARGETS rfw DESTINATION bin)
