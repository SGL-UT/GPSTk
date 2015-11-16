# apps/positioning/CMakeListt.txt

add_executable(PRSolve PRSolve.cpp)
target_link_libraries(PRSolve gpstk)
install (TARGETS PRSolve DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(posInterp posInterp.cpp)
target_link_libraries(posInterp gpstk)
install (TARGETS posInterp DESTINATION "${CMAKE_INSTALL_BINDIR}")
