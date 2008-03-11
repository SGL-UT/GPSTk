# $Id$
INCLUDES = -I$(srcdir)/../../lib/rxio -I$(srcdir)/../../src
LDADD = ./libmdplib.la ../../lib/rxio/librxio.la ../../src/libgpstk.la

lib_LTLIBRARIES = libmdplib.la
libmdplib_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
libmdplib_la_SOURCES = MDPProcessors.cpp SummaryProc.cpp TrackProc.cpp \
NavProc.cpp

bin_PROGRAMS = mdp2rinex mdptool mdpEdit mdpscreenx

mdptool_SOURCES = mdptool.cpp
mdp2rinex_SOURCES = mdp2rinex.cpp
mdpEdit_SOURCES = mdpEdit.cpp
mdpscreenx_SOURCES = mdpscreen.cpp ScreenProc.cpp
mdpscreenx_LDFLAGS = -lcurses
