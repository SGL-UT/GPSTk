# apps/Rinextools/CMakeLists.txt

add_executable(RinDump RinDump.cpp)
target_link_libraries(RinDump gpstk)
install (TARGETS RinDump DESTINATION bin)
add_test(RinDumpTest RinDump)

add_executable(RinEdit RinEdit.cpp)
target_link_libraries(RinEdit gpstk)
install (TARGETS RinEdit DESTINATION bin)
add_test(RinEditTest RinEdit)

add_executable(RinNav RinNav.cpp)
target_link_libraries(RinNav gpstk)
install (TARGETS RinNav DESTINATION bin)
add_test(RinNavTest RinNav)

add_executable(RinSum RinSum.cpp)
target_link_libraries(RinSum gpstk)
install (TARGETS RinSum DESTINATION bin)
add_test(RinSumeTest RinSum)

