# apps/Rinextools/CMakeLists.txt

add_executable(RinDump RinDump.cpp)
target_link_libraries(RinDump gpstk)
install (TARGETS RinDump DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(RinEdit RinEdit.cpp)
target_link_libraries(RinEdit gpstk)
install (TARGETS RinEdit DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(RinSum RinSum.cpp)
target_link_libraries(RinSum gpstk)
install (TARGETS RinSum DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(scanBrdcFile scanBrdcFile.cpp)
target_link_libraries(scanBrdcFile gpstk)
install (TARGETS scanBrdcFile DESTINATION "${CMAKE_INSTALL_BINDIR}")
