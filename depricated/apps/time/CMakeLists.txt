# apps/time/CMakeLists.txt

add_executable(calgps_svg calgps_svg.cpp)
target_link_libraries(calgps_svg gpstk)
install (TARGETS calgps_svg DESTINATION "${CMAKE_INSTALL_BINDIR}")