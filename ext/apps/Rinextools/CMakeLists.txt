# apps/Rinextools/CMakeLists.txt

add_executable(RinDump RinDump.cpp)
target_link_libraries(RinDump gpstk)
install (TARGETS RinDump DESTINATION ${CMAKE_INSTALL_BINDIR} EXPORT ${EXPORT_TARGETS_FILENAME})
#add_test(RinDumpTest RinDump)

add_executable(RinEdit RinEdit.cpp)
target_link_libraries(RinEdit gpstk)
install (TARGETS RinEdit DESTINATION ${CMAKE_INSTALL_BINDIR} EXPORT ${EXPORT_TARGETS_FILENAME})
#add_test(RinEditTest RinEdit)

add_executable(RinNav RinNav.cpp)
target_link_libraries(RinNav gpstk)
install (TARGETS RinNav DESTINATION ${CMAKE_INSTALL_BINDIR} EXPORT ${EXPORT_TARGETS_FILENAME})
#add_test(RinNavTest RinNav)

add_executable(RinSum RinSum.cpp msecHandler.cpp)
target_link_libraries(RinSum gpstk)
install (TARGETS RinSum DESTINATION ${CMAKE_INSTALL_BINDIR} EXPORT ${EXPORT_TARGETS_FILENAME})
#add_test(RinSumeTest RinSum)

