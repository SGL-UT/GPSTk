# apps/converters/CMakeLists.txt

set (PROJECT_SOURCE_DIR "../..")

include_directories(
${PROJECT_SOURCE_DIR}/src
${PROJECT_SOURCE_DIR}/lib/rxio
)

link_directories(
${PROJECT_SOURCE_DIR}/src
${PROJECT_SOURCE_DIR}/lib/rxio
)

add_executable(novaRinex novaRinex.cpp)
target_link_libraries(novaRinex gpstk rxio)
install (TARGETS novaRinex DESTINATION bin)

add_executable(ash2mdp ash2mdp.cpp)
target_link_libraries(ash2mdp gpstk rxio)
install (TARGETS ash2mdp DESTINATION bin)

add_executable(ash2xyz ash2xyz.cpp)
target_link_libraries(ash2xyz gpstk rxio)
install (TARGETS ash2xyz DESTINATION bin)


