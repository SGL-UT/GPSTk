# apps/difftools/CMakeLists.txt

add_executable(rowdiff rowdiff.cpp)
target_link_libraries(rowdiff gpstk)
install (TARGETS rowdiff DESTINATION bin)

add_executable(rnwdiff rnwdiff.cpp)
target_link_libraries(rnwdiff gpstk)
install (TARGETS rnwdiff DESTINATION bin)

add_executable(rmwdiff rmwdiff.cpp)
target_link_libraries(rmwdiff gpstk)
install (TARGETS rmwdiff DESTINATION bin)




