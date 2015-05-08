# apps/converters/CMakeLists.txt

add_executable(novaRinex novaRinex.cpp)
target_link_libraries(novaRinex gpstk)
install (TARGETS novaRinex DESTINATION "${CMAKE_INSTALL_BINDIR}")



