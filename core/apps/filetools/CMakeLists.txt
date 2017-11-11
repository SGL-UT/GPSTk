# apps/filetools/CMakeLists.txt

add_executable(bc2sp3 bc2sp3.cpp)
target_link_libraries(bc2sp3 gpstk)
install (TARGETS bc2sp3 DESTINATION "${CMAKE_INSTALL_BINDIR}")


