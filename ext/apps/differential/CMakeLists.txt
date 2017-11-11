# apps/differential/CMakeLists.txt

add_executable(vecsol vecsol.cpp)
target_link_libraries(vecsol gpstk)
install (TARGETS vecsol DESTINATION "${CMAKE_INSTALL_BINDIR}")
