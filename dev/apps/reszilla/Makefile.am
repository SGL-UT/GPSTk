INCLUDES = -I../../src -I../MDPtools
LDADD = ./librlib.la ./libdatalib.la ../../src/libgpstk.la ../MDPtools/libmdplib.la

lib_LTLIBRARIES = librlib.la libdatalib.la
libdatalib_la_SOURCES = ObsUtils.cpp FFIdentifier.cpp ObsReader.cpp \
	EphReader.cpp MetReader.cpp OrdApp.cpp OrdEngine.cpp \
	../../apps/MDPtools/MDP2Rinex.cpp
librlib_la_SOURCES = util.cpp readers.cpp DDEpoch.cpp ordUtils.cpp \
	PhaseCleaner.cpp PhaseResidual.cpp RobustLinearEstimator.cpp \
	../../apps/MDPtools/MDPHeader.cpp \
	../../apps/MDPtools/MDPObsEpoch.cpp \
	../../apps/MDPtools/MDPNavSubframe.cpp


bin_PROGRAMS = reszilla

reszilla_SOURCES = reszilla.cpp
