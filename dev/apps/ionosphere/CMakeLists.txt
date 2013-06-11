# apps/ionosphere/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/geomatics
)

add_executable(IonoBias IonoBias.cpp)
target_link_libraries(IonoBias gpstk geomatics)

add_executable(TECMaps TECMaps.cpp VTECMap.cpp)
target_link_libraries(TECMaps gpstk geomatics)

