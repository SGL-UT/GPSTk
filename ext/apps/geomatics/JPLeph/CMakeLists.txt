# apps/geomatics/JPLeph/CMakeLists.txt

add_executable(convertSSEph convertSSEph.cpp)
target_link_libraries(convertSSEph gpstk)
install (TARGETS convertSSEph DESTINATION ${CMAKE_INSTALL_BINDIR} EXPORT ${EXPORT_TARGETS_FILENAME})
#add_test(convertSSEphTest convertSSEph)

add_executable(testSSEph testSSEph.cpp)
target_link_libraries(testSSEph gpstk)
install (TARGETS testSSEph DESTINATION ${CMAKE_INSTALL_BINDIR} EXPORT ${EXPORT_TARGETS_FILENAME})
#add_test(testSSEphTest testSSEph)


