# Makefile to build Jam with Mingw GCC on Win32 systems
#
# To use it, you must be in the top Jam source directory,
# have GCC compiler in your current path, and call:
#
#    set JAM_TOOLSET=MINGW
#    make -f builds/win32-gcc.mk
#
# the program "jam.exe" will be created in the new
# directory named "bin.ntx86"
#

CC     = gcc
TARGET = -o jam0.exe
CFLAGS = -DNT

all: jam0
	attrib -r jambase.c
	jam0

include common.mk

