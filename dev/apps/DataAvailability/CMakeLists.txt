# apps/DataAvailability/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/rxio
)

add_executable(daa daa.cpp DataAvailabilityAnalyzer.cpp)
target_link_libraries(daa rxio)
install (TARGETS daa DESTINATION bin)