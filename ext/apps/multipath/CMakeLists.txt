# apps/multipath/CMakeLists.txt

set(APP_MULTIPATH_SOURCE_DIR ${CMAKE_SOURCE_DIR}/core/apps/multipath)
include_directories(${APP_MULTIPATH_SOURCE_DIR})
add_executable(mpsolve_svg
   mpsolve_svg.cpp
   ${APP_MULTIPATH_SOURCE_DIR}/DenseBinnedStats.hpp
   ${APP_MULTIPATH_SOURCE_DIR}/SparseBinnedStats.hpp
   ${APP_MULTIPATH_SOURCE_DIR}/ObsArray.hpp
   ${APP_MULTIPATH_SOURCE_DIR}/ObsArray.cpp
   ${APP_MULTIPATH_SOURCE_DIR}/ValarrayUtils.hpp)
target_link_libraries(mpsolve_svg gpstk)
install (TARGETS mpsolve_svg DESTINATION "${CMAKE_INSTALL_BINDIR}")
