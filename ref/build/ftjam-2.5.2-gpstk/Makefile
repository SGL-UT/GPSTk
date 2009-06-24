# Makefile to build Jam on Unix with the default compiler
# (i.e. "cc" or "$CC" when defined)
#
# To use it, you must be in the top Jam source directory,
# and call
#
#    make
#
# the program "jam" will be created in the new
# directory named "bin.unix"
#

CC      = gcc
TARGET  = -o jam0
CFLAGS  = -g -O2 

all: jam0
	chmod a+w jambase.c
	./jam0

include common.mk

############################################################################
#
# The following is very specific and deals with installation
# it implements "make install" and "make uninstall" for the
# Jam executable.
#
# Note that for now, no documentation is installed with the executable
#

BUILD_DIR    := bin.unix


prefix       := /usr/local
exec_prefix  := ${prefix}
libdir       := ${exec_prefix}/lib
bindir       := ${exec_prefix}/bin
includedir   := ${prefix}/include
datadir      := ${prefix}/share

version_info := @version_info@

DELETE       := rm -f
DELDIR       := rmdir

# The Jam executable name. This is 'jam' on Unix, except Cygwin where
# it will be "jam.exe". Yuckk..
#
JAMEXE       := jam

INSTALL         := /usr/bin/install -c
INSTALL_PROGRAM := ${INSTALL}
MKINSTALLDIRS   := builds/unix/mkinstalldirs


.PHONY: install uninstall check clean distclean

# Unix installation and deinstallation targets.
# Package managers can use the DESTDIR variable to force another
# installation prefix
#
install: jam0
	$(MKINSTALLDIRS) $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) $(BUILD_DIR)/$(JAMEXE) $(DESTDIR)$(bindir)/$(JAMEXE)


uninstall:
	-$(DELETE) $(DESTDIR)$(bindir)/$(JAMEXE)

clean:
	-$(DELETE) $(BUILD_DIR)/*
	-$(DELETE) jam0

distclean: clean
	-$(DELETE) config.log config.status Makefile

check:
	@echo There is no validation suite for this package.
