# Makefile to build Jam with Borland C on Win32 systems
#
# To use it, you must be in the top Jam source directory,
# have the Borland C++ compiler in your current path, and
# call:
#
#    make -fbuilds\win32-borlandc.mk
#
# the program "jam.exe" will be created in the new
# directory named "bin.ntx86"
#

CC     = bcc32
TARGET = -ejam0
CFLAGS = /DNT -w- -q

all: jam0
	attrib -r jambase.c
	jam0

!include common.mk
