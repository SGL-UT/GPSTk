# apps/positioning/CMakeListt.txt

add_executable(poscvt poscvt.cpp)
target_link_libraries(poscvt gpstk)
install (TARGETS poscvt DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(PRSolve PRSolve.cpp)
target_link_libraries(PRSolve gpstk)
install (TARGETS PRSolve DESTINATION "${CMAKE_INSTALL_BINDIR}")
