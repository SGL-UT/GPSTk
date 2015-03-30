# apps/multipath/CMakeLists.txt


add_executable(mpsolve mpsolve.cpp ObsArray.cpp)
target_link_libraries(mpsolve gpstk)
install (TARGETS mpsolve DESTINATION ${CMAKE_INSTALL_BINDIR} EXPORT ${EXPORT_TARGETS_FILENAME})
#add_test(mpsolveTest mpsolve)

