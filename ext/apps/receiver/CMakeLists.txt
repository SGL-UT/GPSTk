# apps/receiver/CMakeLists.txt

add_executable(rtAshtech AshtechMessage.cpp rtAshtech.cpp ScreenControl.cpp)
target_link_libraries(rtAshtech gpstk)
install (TARGETS rtAshtech DESTINATION "${CMAKE_INSTALL_BINDIR}")

