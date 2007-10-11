# $Id$
INCLUDES = -I$(srcdir)/../../src
lib_LTLIBRARIES = librxio.la
librxio_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
librxio_la_SOURCES = AshtechData.cpp AshtechMBEN.cpp AshtechPBEN.cpp \
AshtechEPB.cpp AshtechALB.cpp EphReader.cpp FFIdentifier.cpp \
MDPHeader.cpp MDPNavSubframe.cpp MDPObsEpoch.cpp MDPPVTSolution.cpp \
MDPSelftestStatus.cpp MetReader.cpp ObsReader.cpp ObsUtils.cpp \
RinexConverters.cpp

incldir = $(includedir)/gpstk
incl_HEADERS = AshtechData.hpp AshtechMBEN.hpp AshtechPBEN.hpp \
AshtechStream.hpp DataStatus.hpp EphReader.hpp \
FFIdentifier.hpp MDPHeader.hpp MDPNavSubframe.hpp \
MDPObsEpoch.hpp MDPPVTSolution.hpp MDPSelftestStatus.hpp MetReader.hpp \
ObsReader.hpp ObsUtils.hpp RinexConverters.hpp \
miscenum.hpp

