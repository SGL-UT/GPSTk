# apps/geomatics/kalman/CMakeLists.txt

add_executable(mergeSRI mergeSRI.cpp)
target_link_libraries(mergeSRI gpstk)
install (TARGETS mergeSRI DESTINATION ${CMAKE_INSTALL_BINDIR} EXPORT ${EXPORT_TARGETS_FILENAME})
#add_test(mergeSRITest mergeSRI)

add_executable(tkalm tkalm.cpp)
target_link_libraries(tkalm gpstk)
install (TARGETS tkalm DESTINATION ${CMAKE_INSTALL_BINDIR} EXPORT ${EXPORT_TARGETS_FILENAME})
#add_test(tkalmTest tkalm)

