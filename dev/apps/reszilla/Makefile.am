# $Id$
INCLUDES = -I$(srcdir)/../../lib/rxio -I$(srcdir)/../../src
LDADD = ./librlib.la ../../lib/rxio/librxio.la ../../src/libgpstk.la

lib_LTLIBRARIES = librlib.la
librlib_la_SOURCES =  DDEpoch.cpp PhaseCleaner.cpp PhaseResidual.cpp \
RobustLinearEstimator.cpp SvElevationMap.cpp CycleSlipList.cpp OrdApp.cpp \
OrdEngine.cpp 

bin_PROGRAMS = ordGen ordClock ordEdit ordStats ordLinEst ddGen

ordGen_SOURCES = ordGen.cpp
ordClock_SOURCES = ordClock.cpp
ordEdit_SOURCES = ordEdit.cpp
ordStats_SOURCES = ordStats.cpp
ordLinEst_SOURCES = ordLinEst.cpp
ddGen_SOURCES = ddGen.cpp

