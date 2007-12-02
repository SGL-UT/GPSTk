# $Id: Makefile.am 948 2007-12-01 23:38:22Z architest $

# please add in alphabetic order
INCLUDES = -I$(srcdir)/../../src/
lib_LTLIBRARIES = libprocframe.la
libprocframe_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
libprocframe_la_SOURCES = ComputeIURAWeights.cpp ComputeMOPSWeights.cpp \
ComputeWindUp.cpp 

incldir = $(includedir)/gpstk
incl_HEADERS = ComputeIURAWeights.hpp ComputeMOPSWeights.hpp \
ComputeWindUp.hpp 