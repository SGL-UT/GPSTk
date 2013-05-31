# apps/checktools/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
)

add_executable(rowcheck rowcheck.cpp)
target_link_libraries(rowcheck gpstk)

add_executable(rmwcheck rmwcheck.cpp)
target_link_libraries(rmwcheck gpstk)

add_executable(rnwcheck rnwcheck.cpp)
target_link_libraries(rnwcheck gpstk)

add_executable(ficcheck ficcheck.cpp)
target_link_libraries(ficcheck gpstk)

add_executable(ficacheck ficacheck.cpp)
target_link_libraries(ficacheck gpstk)

