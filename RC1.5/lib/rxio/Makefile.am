# $Id$
# please add new entries in alphabetic order
INCLUDES = -I$(srcdir)/../../src
lib_LTLIBRARIES = librxio.la
librxio_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
librxio_la_SOURCES = AshtechALB.cpp AshtechData.cpp AshtechEPB.cpp \
AshtechMBEN.cpp AshtechPBEN.cpp ATSData.cpp EphReader.cpp FFIdentifier.cpp \
MDPHeader.cpp MDPNavSubframe.cpp MDPObsEpoch.cpp MDPPVTSolution.cpp \
MDPSelftestStatus.cpp MetReader.cpp NovatelData.cpp ObsReader.cpp \
ObsUtils.cpp RinexConverters.cpp

incldir = $(includedir)/gpstk
incl_HEADERS = AshtechALB.hpp AshtechData.hpp AshtechEPB.hpp \
AshtechMBEN.hpp AshtechPBEN.hpp \
AshtechStream.hpp ATSData.hpp ATSStream.hpp DataStatus.hpp EphReader.hpp \
FFIdentifier.hpp MDPHeader.hpp MDPNavSubframe.hpp \
MDPObsEpoch.hpp MDPPVTSolution.hpp MDPSelftestStatus.hpp MDPStream.hpp \
MetReader.hpp NovatelData.hpp NovatelStream.hpp \
ObsReader.hpp ObsUtils.hpp RinexConverters.hpp miscdefs.hpp \
miscenum.hpp

