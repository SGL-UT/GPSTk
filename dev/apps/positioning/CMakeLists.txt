# apps/positioning/CMakeListt.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
)

add_executable(rinexpvt rinexpvt.cpp)
target_link_libraries(rinexpvt gpstk)

add_executable(poscvt poscvt.cpp)
target_link_libraries(poscvt gpstk)

add_executable(PRSolve PRSolve.cpp)
target_link_libraries(PRSolve gpstk)

add_executable(posInterp posInterp.cpp)
target_link_libraries(posInterp gpstk)

add_executable(posmsc posmsc.cpp)
target_link_libraries(posmsc gpstk)

