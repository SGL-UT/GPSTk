# apps/multipath/CMakeLists.txt


add_executable(mpsolve mpsolve.cpp ObsArray.cpp)
target_link_libraries(mpsolve gpstk)
install (TARGETS mpsolve DESTINATION "${CMAKE_INSTALL_BINDIR}")

