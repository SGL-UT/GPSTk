# apps/CMakeLists.txt

add_subdirectory (filetools)
add_subdirectory (geomatics)
add_subdirectory (misc)

# If on UNIX, do these
if (UNIX)
    add_subdirectory (rfw)
endif (UNIX)

add_subdirectory (Rinextools)


