# $Id$
INCLUDES = -I$(srcdir)/../../src -I$(srcdir)/../../lib/rxio
LDADD = ../../src/libgpstk.la ../../lib/rxio/librxio.la

bin_PROGRAMS = svvis wheresat findMoreThan12 compStaVis compSatVis DOPcalc

svvis_SOURCES = svvis.cpp
wheresat_SOURCES = WhereSat.cpp
findMoreThan12_SOURCES = findMoreThan12.cpp
compStaVis_SOURCES = compStaVis.cpp VisSupport.cpp StaStats.cpp
compSatVis_SOURCES = compSatVis.cpp VisSupport.cpp StaStats.cpp
DOPcalc_SOURCES = DOPcalc.cpp

