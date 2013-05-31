# apps/converters/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/rxio
)

add_executable(novaRinex novaRinex.cpp)
target_link_libraries(novaRinex gpstk rxio)

add_executable(ash2mdp ash2mdp.cpp)
target_link_libraries(ash2mdp gpstk rxio)

add_executable(ash2xyz ash2xyz.cpp)
target_link_libraries(ash2xyz gpstk rxio)

#add_executable(RinexConvert RinexConvert.cpp)
#target_link_libraries(RinexConvert gpstk)

