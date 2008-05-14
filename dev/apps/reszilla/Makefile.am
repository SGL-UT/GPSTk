# $Id$
INCLUDES = -I$(srcdir)/../../src -I$(srcdir)/../../lib/rxio \
	-I$(srcdir)/../../lib/geomatics
LDADD = ./librlib.la ../../lib/rxio/librxio.la ../../src/libgpstk.la

lib_LTLIBRARIES = librlib.la
librlib_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
librlib_la_SOURCES = DDEpoch.cpp PhaseCleaner.cpp PhaseResidual.cpp \
	RobustLinearEstimator.cpp SvElevationMap.cpp CycleSlipList.cpp \
        OrdApp.cpp OrdEngine.cpp

bin_PROGRAMS = ordGen ordClock ordLinEst ordEdit ordStats ddGen

ordGen_SOURCES = ordGen.cpp
ordClock_SOURCES = ordClock.cpp
ordEdit_SOURCES = ordEdit.cpp
ordLinEst_SOURCES = ordLinEst.cpp
ordStats_SOURCES = ordStats.cpp
ddGen_SOURCES = ddGen.cpp

