# $Id$
INCLUDES = -I$(srcdir)/../../src -I$(srcdir)/../../lib/rxio
LDADD = ../../src/libgpstk.la ../../lib/rxio/librxio.la

bin_PROGRAMS = rfw rtAshtech ash2mdp

rfw_SOURCES = rfw.cpp
rtAshtech_SOURCES = AshtechMessage.cpp rtAshtech.cpp ScreenControl.cpp
ash2mdp_SOURCES = ash2mdp.cpp
