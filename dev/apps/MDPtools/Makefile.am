# $Id: Makefile.am 2143 2009-09-28 12:26:55Z ocibu $
AM_CPPFLAGS = -I$(srcdir)/../../lib/rxio -I$(srcdir)/../../src
LDADD = ./libmdplib.la ../../lib/rxio/librxio.la ../../src/libgpstk.la

lib_LTLIBRARIES = libmdplib.la
libmdplib_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
libmdplib_la_SOURCES = BELogEntry.cpp MDPProcessors.cpp SummaryProc.cpp TrackProc.cpp \
NavProc.cpp SubframeProc.cpp UniqueAlmStore.cpp

bin_PROGRAMS = mdp2rinex mdptool mdpEdit mdp2fic mdpScrubber #mdpscreenx

mdptool_SOURCES = mdptool.cpp CNAVMessageAsm.cpp
mdp2rinex_SOURCES = mdp2rinex.cpp
mdpEdit_SOURCES = mdpEdit.cpp
mdpScrubber_SOURCES = mdpScrubber.cpp
mdp2fic_SOURCES = mdp2fic.cpp
#mdpscreenx_SOURCES = mdpscreen.cpp ScreenProc.cpp
#mdpscreenx_LDFLAGS = -lcurses
