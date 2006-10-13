INCLUDES = -I../../src
LDADD = ./libmdplib.la ../../src/libgpstk.la
AM_LDFLAGS = -lncurses

noinst_LTLIBRARIES = libmdplib.la
libmdplib_la_SOURCES = MDP2Rinex.cpp MDPHeader.cpp MDPNavSubframe.cpp MDPObsEpoch.cpp MDPProcessors.cpp MDPPVTSolution.cpp MDPSelftestStatus.cpp NavProc.cpp mdpscreen/ScreenProc.cpp SummaryProc.cpp TCPStream.cpp TrackProc.cpp 
noinst_PROGRAMS = mdp2rinex mdptool tcptest

mdp2rinex_SOURCES = mdp2rinex.cpp
mdptool_SOURCES = mdptool.cpp
tcptest_SOURCES = tcptest.cpp
