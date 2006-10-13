INCLUDES = -I../../src -I../MDPtools
LDADD = ./librlib.la ../MDPtools/libmdplib.la ../../src/libgpstk.la

noinst_LTLIBRARIES = librlib.la
librlib_la_SOURCES = util.cpp readers.cpp DDEpoch.cpp ordUtils.cpp PhaseCleaner.cpp PhaseResidual.cpp RobustLinearEstimator.cpp

noinst_PROGRAMS = reszilla

reszilla_SOURCES = reszilla.cpp
