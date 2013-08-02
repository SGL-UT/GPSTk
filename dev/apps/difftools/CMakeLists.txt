# apps/difftools/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
)

add_executable(rowdiff rowdiff.cpp)
target_link_libraries(rowdiff gpstk)
install (TARGETS rowdiff DESTINATION bin)

add_executable(rnwdiff rnwdiff.cpp)
target_link_libraries(rnwdiff gpstk)
install (TARGETS rnwdiff DESTINATION bin)

add_executable(rmwdiff rmwdiff.cpp)
target_link_libraries(rmwdiff gpstk)
install (TARGETS rmwdiff DESTINATION bin)

add_executable(ephdiff ephdiff.cpp)
target_link_libraries(ephdiff gpstk)
install (TARGETS ephdiff DESTINATION bin)

add_executable(ficdiff ficdiff.cpp)
target_link_libraries(ficdiff gpstk)
install (TARGETS ficdiff DESTINATION bin)

