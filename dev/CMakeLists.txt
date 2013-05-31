cmake_minimum_required(VERSION 2.8)

project (gpstk)

message ("system = ${CMAKE_SYSTEM}")
message ("system_name = ${CMAKE_SYSTEM_NAME}")

add_subdirectory (src)
add_subdirectory (lib)
add_subdirectory (apps)
add_subdirectory (examples)
