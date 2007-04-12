# $Id$
INCLUDES = -I$(srcdir)/../../lib/rxio -I$(srcdir)/../../src
LDADD = ./libmdplib.la ../../lib/rxio/librxio.la ../../src/libgpstk.la

lib_LTLIBRARIES = libmdplib.la
libmdplib_la_SOURCES = MDPProcessors.cpp SummaryProc.cpp TrackProc.cpp \
NavProc.cpp

bin_PROGRAMS = mdp2rinex mdptool tcptest

mdptool_SOURCES = mdptool.cpp
mdp2rinex_SOURCES = mdp2rinex.cpp
tcptest_SOURCES = tcptest.cpp
