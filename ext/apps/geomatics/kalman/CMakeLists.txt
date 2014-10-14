# apps/geomatics/kalman/CMakeLists.txt

add_executable(mergeSRI mergeSRI.cpp)
target_link_libraries(mergeSRI gpstk)
install (TARGETS mergeSRI DESTINATION bin)
add_test(mergeSRITest mergeSRI)

add_executable(tkalm tkalm.cpp)
target_link_libraries(tkalm gpstk)
install (TARGETS tkalm DESTINATION bin)
add_test(tkalmTest tkalm)

