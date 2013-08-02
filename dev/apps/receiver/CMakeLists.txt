# apps/receiver/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/rxio
# FIX THIS DEPENDENCY
${CMAKE_SOURCE_DIR}/apps/rfw
)

add_executable(rtAshtech AshtechMessage.cpp rtAshtech.cpp ScreenControl.cpp)
target_link_libraries(rtAshtech gpstk rxio)
install (TARGETS rtAshtech DESTINATION bin)

