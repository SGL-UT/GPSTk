# apps/checktools/CMakeLists.txt

add_executable(rowcheck rowcheck.cpp)
target_link_libraries(rowcheck gpstk)
install (TARGETS rowcheck DESTINATION bin)
add_test(rowcheckTest rowcheck)

add_executable(rmwcheck rmwcheck.cpp)
target_link_libraries(rmwcheck gpstk)
install (TARGETS rmwcheck DESTINATION bin)
add_test(rmwcheckTest rmwcheck)

add_executable(rnwcheck rnwcheck.cpp)
target_link_libraries(rnwcheck gpstk)
install (TARGETS rnwcheck DESTINATION bin)
add_test(rnwcheckTest rnwcheck)

