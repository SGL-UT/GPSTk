# apps/checktools/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
)

add_executable(rowcheck rowcheck.cpp)
target_link_libraries(rowcheck gpstk)
install (TARGETS rowcheck DESTINATION bin)

add_executable(rmwcheck rmwcheck.cpp)
target_link_libraries(rmwcheck gpstk)
install (TARGETS rmwcheck DESTINATION bin)

add_executable(rnwcheck rnwcheck.cpp)
target_link_libraries(rnwcheck gpstk)
install (TARGETS rnwcheck DESTINATION bin)

add_executable(ficcheck ficcheck.cpp)
target_link_libraries(ficcheck gpstk)
install (TARGETS ficcheck DESTINATION bin)

add_executable(ficacheck ficacheck.cpp)
target_link_libraries(ficacheck gpstk)
install (TARGETS ficacheck DESTINATION bin)

