# $Id$
INCLUDES = -I$(srcdir)/../../src -I$(srcdir)/../../lib/geomatics -I$(srcdir)/../../lib/vdraw -I$(srcdir)/../../lib/vplot
LDADD = ../../src/libgpstk.la ../../lib/vdraw/libvdraw.la ../../lib/vdraw/libvplot.la ../../lib/vdraw/libgeomatics.la

bin_PROGRAMS = mpsolve

mpsolve_SOURCES = mpsolve.cpp ObsArray.cpp
