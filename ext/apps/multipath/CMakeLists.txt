# apps/multipath/CMakeLists.txt

add_executable(mpsolve
   mpsolve.cpp
   DenseBinnedStats.hpp
   SparseBinnedStats.hpp
   ObsArray.hpp
   ObsArray.cpp
   ValarrayUtils.hpp)
target_link_libraries(mpsolve gpstk)
install (TARGETS mpsolve DESTINATION "${CMAKE_INSTALL_BINDIR}")
