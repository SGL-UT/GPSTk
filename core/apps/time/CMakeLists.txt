# apps/time/CMakeLists.txt

add_executable(timeconvert timeconvert.cpp)
target_link_libraries(timeconvert gpstk)
install (TARGETS timeconvert DESTINATION "${CMAKE_INSTALL_BINDIR}")


