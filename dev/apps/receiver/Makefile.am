# $Id$
INCLUDES = -I$(srcdir)/../../src -I$(srcdir)/../../lib/rxio -I../rfw
LDADD = ../../src/libgpstk.la ../../lib/rxio/librxio.la ../rfw/FDStreamBuff.o ../rfw/TCPStreamBuff.o

bin_PROGRAMS = rtAshtech ash2mdp

rtAshtech_SOURCES = AshtechMessage.cpp rtAshtech.cpp ScreenControl.cpp
ash2mdp_SOURCES = ash2mdp.cpp
