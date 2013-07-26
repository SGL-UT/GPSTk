cmake_minimum_required(VERSION 2.8)

project (gpstk)

message ("system = ${CMAKE_SYSTEM}")
message ("system_name = ${CMAKE_SYSTEM_NAME}")

set (CMAKE_INSTALL_PREFIX /home/bparsons/git/gpstk)
set (GPSTK_SO_VERSION 21)

 if (UNIX)
	 set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -shared")
	 if (APPLE)
	 	set (CMAKE_SHARED_LIBRARY_SUFFIX .dylib)
	 endif (APPLE)
elseif (WIN32)
	if (MSVC12)
		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_DEPRECATE -wd4274 -wd4503")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 -DWIN32 -D_CRT_SECURE_NO_DEPRECATE -D_USE_MATH_DEFINES /EHsc /GR -wd4274 -wd4503")
	elseif (MSVC10)
		set (include_directories("C:\Program Files (x86)\GnuWin32\lib"))
		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_DEPRECATE -wd4274 -wd4503")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 -DWIN32 -D_CRT_SECURE_NO_DEPRECATE -D_USE_MATH_DEFINES /EHsc /GR -wd4274 -wd4503")
	endif (MSVC12)		
endif(UNIX)

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")



if (NOT ${CMAKE_COMPILER_IS_GNUCC} )
	set(NEED_GETOPT TRUE)
endif(NOT ${CMAKE_COMPILER_IS_GNUCC} )

#Debug script - uncomment for printing of all cmake variables
get_cmake_property(_variableNames VARIABLES)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

add_subdirectory (src)
add_subdirectory (lib)
add_subdirectory (apps)
add_subdirectory (examples)
