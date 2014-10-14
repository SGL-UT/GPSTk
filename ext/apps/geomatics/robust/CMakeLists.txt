# apps/geomatics/robust/CMakeLists.txt

add_executable(lsfilt lsfilt.cpp)
target_link_libraries(lsfilt gpstk)
install (TARGETS lsfilt DESTINATION bin)
add_test(lsfiltTest lsfilt)

add_executable(rstats rstats.cpp)
target_link_libraries(rstats gpstk)
install (TARGETS rstats DESTINATION bin)
add_test(rstatsTest rstats)

