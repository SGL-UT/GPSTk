# apps/Rinextools/CMakeLists.txt

add_executable(RinDump RinDump.cpp)
target_link_libraries(RinDump gpstk)
install (TARGETS RinDump DESTINATION "${CMAKE_INSTALL_BINDIR}")
