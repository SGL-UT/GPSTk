# apps/CMakeLists.txt

add_subdirectory (checktools)
add_subdirectory (clocktools)
add_subdirectory (converters)
add_subdirectory (DataAvailability)
add_subdirectory (differential)
add_subdirectory (difftools)
add_subdirectory (filetools)
add_subdirectory (geomatics)
add_subdirectory (ionosphere)
add_subdirectory (mergetools)

# If on UNIX, do these
if (UNIX)
	add_subdirectory (MDPtools)
	add_subdirectory (rfw)
	add_subdirectory (swrx)
	#add_subdirectory (receiver)
endif (UNIX)

add_subdirectory (multipath)
add_subdirectory (positioning)
add_subdirectory (performance)
add_subdirectory (reszilla)
add_subdirectory (Rinextools)
add_subdirectory (time)
add_subdirectory (visibility)

