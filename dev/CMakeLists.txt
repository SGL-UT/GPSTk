# Minimum Required CMake Version
cmake_minimum_required(VERSION 2.8.4)

# Project Name
project (gpstk)

#----------------------------------------
# System Variables
#----------------------------------------

message ("system = ${CMAKE_SYSTEM}")
message ("system_name = ${CMAKE_SYSTEM_NAME}")

#----------------------------------------
# Installation Path Prefix - Change to desired location
#----------------------------------------

set (CMAKE_INSTALL_PREFIX $ENV{gpstk})

#----------------------------------------
# Set Compiler options
#----------------------------------------

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

#----------------------------------------
# Set Build path options
#----------------------------------------

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

#----------------------------------------
# NEED_GETOPT flag for Sources
#----------------------------------------

if (NOT "${CMAKE_COMPILER_IS_GNUCC}" )
	set(NEED_GETOPT TRUE)
endif(NOT "${CMAKE_COMPILER_IS_GNUCC}" )

#----------------------------------------
# Debug script - uncomment for printing of all cmake variables
#----------------------------------------

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

#----------------------------------------
# Recursively build core and ext lib
#----------------------------------------

# Recursively search through core/lib and group the following file types
file(GLOB_RECURSE CORE_SRC "core/lib/*.cpp")
file(GLOB_RECURSE CORE_SRC2 "core/lib/*.c")
file(GLOB_RECURSE CORE_HDRS "core/lib/*.h")
file(GLOB_RECURSE CORE_HDRS2 "core/lib/*.hpp")

# Recursively search through ext/lib and group the following file types
file(GLOB_RECURSE EXT_SRC "ext/lib/*.cpp")
file(GLOB_RECURSE EXT_SRC2 "ext/lib/*.c")
file(GLOB_RECURSE EXT_HDRS "ext/lib/*.h")
file(GLOB_RECURSE EXT_HDRS2 "ext/lib/*.hpp")

# Map out the directory structure of core/lib for includes
set (CORE_INCLUDE_DIRS "")
foreach (_headerFile ${CORE_HDRS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND CORE_INCLUDE_DIRS ${_dir})
endforeach()

foreach (_headerFile ${CORE_HDRS2})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND CORE_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES CORE_INCLUDE_DIRS)

# Include the mapped /corelib directory structure
include_directories(${CORE_INCLUDE_DIRS})

# Map out the directory structure of ext/lib for includes
set (EXT_INCLUDE_DIRS "")
foreach (_headerFile ${EXT_HDRS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND EXT_INCLUDE_DIRS ${_dir})
endforeach()

foreach (_headerFile ${EXT_HDRS2})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND EXT_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES EXT_INCLUDE_DIRS)

# Include the mapped ext/lib directory structure
include_directories(${EXT_INCLUDE_DIRS})

# Create the gpstk library using core and ext source files
add_library (gpstk ${STADYN} ${CORE_SRC} ${CORE_SRC2} ${EXT_SRC} ${EXT_SRC2})

#----------------------------------------
# Install the gpstk library and core/ext headers
#----------------------------------------

install (TARGETS gpstk DESTINATION lib)
install (FILES ${CORE_HDRS} ${CORE_HDRS2} ${EXT_HDRS} ${EXT_HDRS2} DESTINATION include )

#----------------------------------------
# Add sub-directories
#----------------------------------------

add_subdirectory (core)
add_subdirectory (ext)
add_subdirectory (examples)

#----------------------------------------
# Experimental stuff to have cmake build debs for us
# To use this, run "make package"
#----------------------------------------
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GPSTk libraries and applications for GNSS processing.")
SET(CPACK_PACKAGE_VENDOR "ARL:UT SGL")
SET(CPACK_PACKAGE_CONTACT "Bryan Parsons")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_PACKAGE_VERSION_MAJOR "2")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "1")
SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY "OFF")

SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.13)")
SET(CPACK_DEBIAN_SECTION "stable")
SET(CPACK_DEBIAN_PACKAGE_SECTION "science")

SET(CPACK_GENERATOR "DEB;TGZ")
INCLUDE(CPack)
