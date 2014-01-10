# apps/clocktools/CMakeLists.txt

add_executable(rmoutlier rmoutlier.cpp)
target_link_libraries(rmoutlier gpstk)
install (TARGETS rmoutlier DESTINATION bin)

