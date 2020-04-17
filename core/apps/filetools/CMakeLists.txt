# apps/filetools/CMakeLists.txt

add_executable(bc2sp3 bc2sp3.cpp)
target_link_libraries(bc2sp3 gpstk)
install (TARGETS bc2sp3 DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(smdscheck smdscheck.cpp)
target_link_libraries(smdscheck gpstk)
install (TARGETS smdscheck DESTINATION "${CMAKE_INSTALL_BINDIR}")


