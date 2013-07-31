# apps/positioning/CMakeListt.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
)

add_executable(rinexpvt rinexpvt.cpp)
target_link_libraries(rinexpvt gpstk)
install (TARGETS rinexpvt DESTINATION bin)

add_executable(poscvt poscvt.cpp)
target_link_libraries(poscvt gpstk)
install (TARGETS poscvt DESTINATION bin)

add_executable(PRSolve PRSolve.cpp)
target_link_libraries(PRSolve gpstk)
install (TARGETS PRSolve DESTINATION bin)

add_executable(posInterp posInterp.cpp)
target_link_libraries(posInterp gpstk)
install (TARGETS posInterp DESTINATION bin)

add_executable(posmsc posmsc.cpp)
target_link_libraries(posmsc gpstk)
install (TARGETS posmsc DESTINATION bin)

