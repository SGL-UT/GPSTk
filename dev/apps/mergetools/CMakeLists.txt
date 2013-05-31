# apps/mergetools/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
)

add_executable(mergeRinObs mergeRinObs.cpp)
target_link_libraries(mergeRinObs gpstk)

add_executable(mergeRinNav mergeRinNav.cpp)
target_link_libraries(mergeRinNav gpstk)

add_executable(mergeRinMet mergeRinMet.cpp)
target_link_libraries(mergeRinMet gpstk)

add_executable(mergeFIC mergeFIC.cpp)
target_link_libraries(mergeFIC gpstk)

