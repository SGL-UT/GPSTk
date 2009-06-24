# Makefile to build jam with Visual C on Win32 systems
#
# To use it, you must be in the top Jam source directory,
# have the compiler in your path, and call:
#
#  nmake -f builds\win32-visualc.mk
#
# the program "jam.exe" will be created in a new directory
# named "bin.ntx86"
#
CC       = cl /nologo
CFLAGS   = -DNT 
TARGET   = /Fejam0
LINKLIBS = oldnames.lib kernel32.lib libc.lib

all: jam0
	attrib -r jambase.c
	jam0

!include common.mk
