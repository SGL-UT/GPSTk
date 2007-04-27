INCLUDES = -I$(srcdir)/../../src -I$(srcdir)/../MDPtools
LDADD = ./librlib.la ./libdatalib.la ../../src/libgpstk.la ../MDPtools/libmdplib.la

lib_LTLIBRARIES = librlib.la libdatalib.la
libdatalib_la_SOURCES = OrdApp.cpp OrdEngine.cpp
librlib_la_SOURCES = DDEpoch.cpp PhaseCleaner.cpp PhaseResidual.cpp \
	RobustLinearEstimator.cpp SvElevationMap.cpp CycleSlipList.cpp 
