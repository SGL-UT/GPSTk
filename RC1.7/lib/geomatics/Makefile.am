# $Id$
# please add in alphabetic order
INCLUDES = -I$(srcdir)/../../src/
lib_LTLIBRARIES = libgeomatics.la
libgeomatics_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
libgeomatics_la_SOURCES = AntexData.cpp AntexHeader.cpp DDid.cpp DiscCorr.cpp \
	EarthOrientation.cpp GSatID.cpp GeodeticFrames.cpp Namelist.cpp \
	PhaseWindup.cpp PreciseRange.cpp RobustStats.cpp SRI.cpp SRIFilter.cpp \
	SRIleastSquares.cpp SatPass.cpp SolarPosition.cpp SolarSystem.cpp \
	SunEarthSatGeometry.cpp format.cpp random.cpp

incldir = $(includedir)/gpstk
incl_HEADERS = AntexBase.hpp AntexData.hpp AntexHeader.hpp AntexStream.hpp DDid.hpp \
	DiscCorr.hpp EarthOrientation.hpp format.hpp GSatID.hpp GeodeticFrames.hpp \
	index.hpp logstream.hpp Namelist.hpp PhaseWindup.hpp PreciseRange.hpp \
	random.hpp RobustStats.hpp SRI.hpp SRIFilter.hpp SRIleastSquares.hpp \
	SRIMatrix.hpp SatPass.hpp SolarPosition.hpp SolarSystem.hpp \
	SunEarthSatGeometry.hpp
