# apps/geomatics/robust/CMakeLists.txt

add_executable(lsfilt lsfilt.cpp)
target_link_libraries(lsfilt gpstk)
install (TARGETS lsfilt DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(rstats rstats.cpp)
target_link_libraries(rstats gpstk)
install (TARGETS rstats DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(smtest smtest.cpp)
target_link_libraries(smtest gpstk)
install (TARGETS smtest DESTINATION "${CMAKE_INSTALL_BINDIR}")

