# apps/CMakeLists.txt

add_executable(calgps calgps.cpp)
target_link_libraries(calgps gpstk)
install (TARGETS calgps DESTINATION "${CMAKE_INSTALL_BINDIR}")
