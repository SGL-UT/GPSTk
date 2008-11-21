# $Id: Makefile.am 1182 2008-04-04 14:07:43Z btolman $
# please add in alphabetic order
INCLUDES = -I$(srcdir)/../../src/ -I.
lib_LTLIBRARIES = libRinex3.la
libRinex3_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
libRinex3_la_SOURCES = CommonTime.cpp GPSEphemerisStore.cpp SP3Data.cpp \
SP3EphemerisStore.cpp SP3Header.cpp TabularEphemerisStore.cpp

incldir = $(includedir)/gpstk
incl_HEADERS = CommonTime.hpp GPSEphemerisStore.hpp \
KeplerianEphemerisStore.hpp SP3EphemerisStore.hpp SP3Stream.hpp \
SP3Data.hpp SP3Header.hpp TabularEphemerisStore.hpp TimeConstants.hpp \
XvtStore.hpp
