INCLUDES = -I../../src
LDADD = ./libMDPtoolslib.la ../../src/libgpstk.la

noinst_LTLIBRARIES = libMDPtoolslib.la
libMDPtoolslib_la_SOURCES = MDPHeader.cpp MDPNavSubframe.cpp MDPObsEpoch.cpp MDPProcessors.cpp MDPPVTSolution.cpp MDPSelftestStatus.cpp FormatConversionFunctions.cpp NavProc.cpp ScreenProc.cpp SummaryProc.cpp TCPStream.cpp TrackProc.cpp

noinst_PROGRAMS = mdp2rinex mdptool miscenum tcptest

mdp2rinex_SOURCES = mdp2rinex.cpp
mdptool_SOURCES = mdptool.cpp
miscenum_SOURCES = miscenum.cpp
tcptest_SOURCES = tcptest.cpp
