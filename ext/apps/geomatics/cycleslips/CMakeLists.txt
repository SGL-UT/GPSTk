# apps/geomatics/cycleslips/CMakeLists.txt

add_executable(DiscFix DiscFix.cpp)
target_link_libraries(DiscFix gpstk)
install (TARGETS DiscFix DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(dfix dfix.cpp)
target_link_libraries(dfix gpstk)
install (TARGETS dfix DESTINATION "${CMAKE_INSTALL_BINDIR}")

