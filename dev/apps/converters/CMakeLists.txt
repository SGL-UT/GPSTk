# apps/converters/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/rxio
)

add_executable(novaRinex novaRinex.cpp)
target_link_libraries(novaRinex rxio)
install (TARGETS novaRinex DESTINATION bin)

add_executable(ash2mdp ash2mdp.cpp)
target_link_libraries(ash2mdp rxio)
install (TARGETS ash2mdp DESTINATION bin)

add_executable(ash2xyz ash2xyz.cpp)
target_link_libraries(ash2xyz rxio)
install (TARGETS ash2xyz DESTINATION bin)


