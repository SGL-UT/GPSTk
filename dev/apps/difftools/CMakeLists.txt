# apps/difftools/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
)

add_executable(rowdiff rowdiff.cpp)
target_link_libraries(rowdiff gpstk)

add_executable(rnwdiff rnwdiff.cpp)
target_link_libraries(rnwdiff gpstk)

add_executable(rmwdiff rmwdiff.cpp)
target_link_libraries(rmwdiff gpstk)

add_executable(ephdiff ephdiff.cpp)
target_link_libraries(ephdiff gpstk)

add_executable(ficdiff ficdiff.cpp)
target_link_libraries(ficdiff gpstk)

