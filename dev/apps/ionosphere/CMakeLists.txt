# apps/ionosphere/CMakeLists.txt

add_executable(IonoBias IonoBias.cpp)
target_link_libraries(IonoBias gpstk)
install (TARGETS IonoBias DESTINATION bin)

add_executable(TECMaps TECMaps.cpp VTECMap.cpp)
target_link_libraries(TECMaps gpstk)
install (TARGETS TECMaps DESTINATION bin)

