# apps/rfw/CMakeLists.txt

add_executable(rfw rfw.cpp FDStreamBuff.cpp TCPStreamBuff.cpp)
target_link_libraries(rfw gpstk)
install (TARGETS rfw DESTINATION bin)
add_test(rfwTest rfw)

if (CMAKE_SYSTEM_NAME MATCHES "SunOS")
  target_link_libraries(rfw socket nsl)
endif()
