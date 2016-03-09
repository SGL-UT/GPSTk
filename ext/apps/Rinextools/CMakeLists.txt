# apps/Rinextools/CMakeLists.txt

add_executable(RinEdit RinEdit.cpp)
target_link_libraries(RinEdit gpstk)
install (TARGETS RinEdit DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(RinNav RinNav.cpp)
target_link_libraries(RinNav gpstk)
install (TARGETS RinNav DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(RinSum RinSum.cpp)
target_link_libraries(RinSum gpstk)
install (TARGETS RinSum DESTINATION "${CMAKE_INSTALL_BINDIR}")

