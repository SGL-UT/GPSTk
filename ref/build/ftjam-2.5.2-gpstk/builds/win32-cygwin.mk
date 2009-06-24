# Makefile to build Jam with Cygwin on Win32 systems
#
# To use it, you must be in the top Jam source directory,
# have GCC compiler in your current path, and call:
#
#    make -f builds/win32-cygwin.mk
#
# the program "jam.exe" will be created in the new
# directory named "bin.cygwinx86"
#
# note that the resulting executable will only be usable
# under Cygwin, since it will output Unix commands. I.e.
# it will _not_ use JAM_TOOLSET and won't be able to compile
# with Mingw, Visual C++, Borland C++ and other native
# Win32 compilers
#

CC     = gcc
TARGET = -o jam0.exe
CFLAGS = -D__cygwin__

all: jam0
	attrib -r jambase.c
	jam0

include common.mk
