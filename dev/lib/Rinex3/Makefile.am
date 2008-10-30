# $Id: Makefile.am 1182 2008-04-04 14:07:43Z btolman $
# please add in alphabetic order
INCLUDES = -I$(srcdir)/../../src/
lib_LTLIBRARIES = libRinex3.la
libRinex3_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
libRinex3_la_SOURCES = GPSEphemerisStore.cpp SP3EphemerisStore.cpp \
TabularEphemerisStore.cpp

incldir = $(includedir)/gpstk
incl_HEADERS = GPSEphemerisStore.hpp KeplerianEphemerisStore.hpp \
SP3EphemerisStore.hpp  TabularEphemerisStore.hpp
