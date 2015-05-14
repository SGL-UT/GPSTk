# apps/geomatics/kalman/CMakeLists.txt

add_executable(mergeSRI mergeSRI.cpp)
target_link_libraries(mergeSRI gpstk)
install (TARGETS mergeSRI DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(tkalm tkalm.cpp)
target_link_libraries(tkalm gpstk)
install (TARGETS tkalm DESTINATION "${CMAKE_INSTALL_BINDIR}")

