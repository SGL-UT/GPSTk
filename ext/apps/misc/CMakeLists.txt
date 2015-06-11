# apps/misc/CMakeLists.txt

add_executable(xSVNumXRef  xSVNumXRef.cpp)
target_link_libraries(xSVNumXRef gpstk)
install (TARGETS xSVNumXRef DESTINATION "${CMAKE_INSTALL_BINDIR}")
