# apps/positioning/CMakeListt.txt

add_executable(posInterp posInterp.cpp)
target_link_libraries(posInterp gpstk)
install (TARGETS posInterp DESTINATION "${CMAKE_INSTALL_BINDIR}")
