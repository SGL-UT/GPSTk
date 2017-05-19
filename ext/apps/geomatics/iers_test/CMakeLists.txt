# apps/geomatics/iers_test/CMakeLists.txt

add_executable(test_EO_SOFA test_EO_SOFA.cpp)
target_link_libraries(test_EO_SOFA gpstk)
install (TARGETS test_EO_SOFA DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(test_tides test_tides.cpp)
target_link_libraries(test_tides gpstk)
install (TARGETS test_tides DESTINATION "${CMAKE_INSTALL_BINDIR}")


