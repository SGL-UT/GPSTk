# apps/CMakeLists.txt

add_subdirectory (clocktools)
add_subdirectory (differential)
add_subdirectory (filetools)
add_subdirectory (geomatics)
add_subdirectory (misc)
add_subdirectory (positioning)
add_subdirectory (time)

# If on UNIX, do these
if (UNIX)
	add_subdirectory (rfw)
	add_subdirectory (swrx)
	#add_subdirectory (receiver)
endif (UNIX)

add_subdirectory (multipath)
add_subdirectory (Rinextools)


