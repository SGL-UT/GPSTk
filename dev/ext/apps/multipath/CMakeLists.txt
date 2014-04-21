# apps/multipath/CMakeLists.txt


add_executable(mpsolve mpsolve.cpp ObsArray.cpp)
target_link_libraries(mpsolve gpstk)
install (TARGETS mpsolve DESTINATION bin)
add_test(mpsolveTest mpsolve)

