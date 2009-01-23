# $Id$
# please add in alphabetic order
INCLUDES = -I$(srcdir)/../../src/ -I.
lib_LTLIBRARIES = libRinex3.la
libRinex3_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
libRinex3_la_SOURCES = ANSITime.cpp CivilTime.cpp CommonTime.cpp \
   EngEphemeris.cpp GPSEphemerisStore.cpp GPSWeek.cpp GPSWeekSecond.cpp \
   GPSWeekZcount.cpp JulianDate.cpp MJD.cpp SP3Data.cpp SP3EphemerisStore.cpp \
   SP3Header.cpp TabularEphemerisStore.cpp TimeSystem.cpp TimeTag.cpp \
   UnixTime.cpp YDSTime.cpp ../../src/SP3SatID.cpp ../../src/Triple.cpp \
   ../../src/ECEF.cpp ../../src/Geodetic.cpp ../../src/TimeConverters.cpp

incldir = $(includedir)/gpstk
incl_HEADERS = CommonTime.hpp GPSEphemerisStore.hpp \
KeplerianEphemerisStore.hpp SP3EphemerisStore.hpp TabularEphemerisStore.hpp
