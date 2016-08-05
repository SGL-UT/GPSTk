# apps/difftools/CMakeLists.txt

add_executable(rowdiff rowdiff.cpp)
target_link_libraries(rowdiff gpstk)
install (TARGETS rowdiff DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(rnwdiff rnwdiff.cpp)
target_link_libraries(rnwdiff gpstk)
install (TARGETS rnwdiff DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(rmwdiff rmwdiff.cpp)
target_link_libraries(rmwdiff gpstk)
install (TARGETS rmwdiff DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(rinheaddiff rinheaddiff.cpp)
target_link_libraries(rinheaddiff gpstk)
install (TARGETS rinheaddiff DESTINATION "${CMAKE_INSTALL_BINDIR}")
