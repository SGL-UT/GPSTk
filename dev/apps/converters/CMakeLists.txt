# apps/converters/CMakeLists.txt

add_executable(novaRinex novaRinex.cpp)
target_link_libraries(novaRinex gpstk)
install (TARGETS novaRinex DESTINATION bin)

add_executable(ash2mdp ash2mdp.cpp)
target_link_libraries(ash2mdp gpstk)
install (TARGETS ash2mdp DESTINATION bin)

add_executable(ash2xyz ash2xyz.cpp)
target_link_libraries(ash2xyz gpstk)
install (TARGETS ash2xyz DESTINATION bin)


