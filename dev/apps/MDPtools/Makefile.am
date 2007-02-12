INCLUDES = -I../../src
LDADD = ./libmdplib.la ../../src/libgpstk.la
AM_LDFLAGS = -lncurses

lib_LTLIBRARIES = libmdplib.la
libmdplib_la_SOURCES = MDPHeader.cpp MDPPVTSolution.cpp MDPNavSubframe.cpp \
MDPObsEpoch.cpp MDPSelftestStatus.cpp RinexConverters.cpp TCPStream.cpp \
MDPProcessors.cpp SummaryProc.cpp TrackProc.cpp NavProc.cpp

bin_PROGRAMS = mdp2rinex mdptool tcptest

mdptool_SOURCES = mdptool.cpp
mdp2rinex_SOURCES = mdp2rinex.cpp
tcptest_SOURCES = tcptest.cpp
