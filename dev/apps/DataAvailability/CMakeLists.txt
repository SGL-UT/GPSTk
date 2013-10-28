# apps/DataAvailability/CMakeLists.txt

add_executable(daa daa.cpp DataAvailabilityAnalyzer.cpp)
target_link_libraries(daa gpstk)
install (TARGETS daa DESTINATION bin)