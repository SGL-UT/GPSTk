# Minimum Required CMake Version
cmake_minimum_required(VERSION 2.8.4)

# Project Name
project (gpstk)

# System Variables
message ("system = ${CMAKE_SYSTEM}")
message ("system_name = ${CMAKE_SYSTEM_NAME}")

# Installation Path Prefix - Change to desired location
set (CMAKE_INSTALL_PREFIX $ENV{gpstk})

if (UNIX) #Unix Compiler Options
	 set (STADYN "SHARED") #Dynamic Libraries Enabled
	 if (APPLE) #Apple Compiler Options
	 	set (CMAKE_SHARED_LIBRARY_SUFFIX .dylib)
		set (CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
	 	set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -shared")
	 elseif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS") #Solaris Compiler Options
	 	set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -mt")
	 	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -compat=5 -erroff=hidevf,wvarhidemem,badargtype2w -lgen -lnsl -lsocket")
	 else (APPLE)
	 	set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -shared")
	 endif (APPLE)
elseif (WIN32) #Windows Compiler Options
	if (MSVC11) #Compiler Options for Microsoft Visual Studio 11 (2012)
		set (STADYN "STATIC") #Static Libraries Enabled
		add_definitions (/MP /D_SCL_SECURE_NO_WARNINGS /D_CRT_SECURE_NO_WARNINGS /D_USE_MATH_DEFINES /EHsc /GR /wd"4274" 
		/wd"4503" /wd"4290" /wd"4267" /wd"4250" /wd"4244" /wd"4101" /wd"4800" /wd"4068")
	elseif (MSVC10) #Compiler Options for Microsoft Visual Studio 10 (2010)
		set (STADYN "STATIC") #Static Libraries Enabled
		include_directories("C:/Program\ Files\ (x86)/GnuWin32/include")
		link_directories("C:/Program\ Files\ (x86)/GnuWin32/lib")
		add_definitions (/MP /D_SCL_SECURE_NO_WARNINGS /D_CRT_SECURE_NO_WARNINGS /D_USE_MATH_DEFINES /EHsc /GR /wd"4274" 
		/wd"4503" /wd"4290" /wd"4267" /wd"4250" /wd"4244" /wd"4101" /wd"4800" /wd"4068")
	endif (MSVC11)		
endif(UNIX)

# Use, i.e. don't skip the full RPATH for the build tree
set (CMAKE_SKIP_BUILD_RPATH FALSE)

# When building, don't use the install RPATH
# (but later on when installing)
set (CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}:$ORIGIN/../lib")

# Add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# The RPATH to be used when installing, but only if it's not a system directory
list (FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}:$ORIGIN/../lib" isSystemDir)
if ("${isSystemDir}" STREQUAL "-1")
   set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}:$ORIGIN/../lib")
endif ("${isSystemDir}" STREQUAL "-1")

# NEED_GETOPT flag for Sources
if (NOT "${CMAKE_COMPILER_IS_GNUCC}" )
	set(NEED_GETOPT TRUE)
endif(NOT "${CMAKE_COMPILER_IS_GNUCC}" )

# Debug script - uncomment for printing of all cmake variables
#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

# Recursively search through lib and group the following file types
file(GLOB_RECURSE SOURCES "lib/*.cpp")
file(GLOB_RECURSE SOURCES2 "lib/*.c")
file(GLOB_RECURSE HEADERS "lib/*.h")
file(GLOB_RECURSE HEADERS2 "lib/*.hpp")

# ap out the directory structure of lib for includes
set (INCLUDE_DIRS "")
foreach (_headerFile ${HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()

foreach (_headerFile ${HEADERS2})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS)

# Include the mapped lib directory structure
include_directories(${INCLUDE_DIRS})

# Create the gpstk library
add_library (gpstk ${STADYN} ${SOURCES} ${SOURCES2})

# Install the gpstk library and headers
install (TARGETS gpstk DESTINATION lib)
install (FILES ${HEADERS} ${HEADERS2} DESTINATION include/gpstk )


# GPSTk Subdirectories
add_subdirectory (apps)
add_subdirectory (examples)

include(CPack)

