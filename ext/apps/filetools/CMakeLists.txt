# apps/filetools/CMakeLists.txt

add_executable(glodump GloDump.cpp)
target_link_libraries(glodump gpstk)
install (TARGETS glodump DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(rinexthin RinexThin.cpp)
target_link_libraries(rinexthin gpstk)
install (TARGETS rinexthin DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(sp3version sp3version.cpp)
target_link_libraries(sp3version gpstk)
install (TARGETS sp3version DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(bc2sp3 bc2sp3.cpp)
target_link_libraries(bc2sp3 gpstk)
install (TARGETS bc2sp3 DESTINATION "${CMAKE_INSTALL_BINDIR}")


