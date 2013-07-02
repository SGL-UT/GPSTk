# $Id: Makefile.am 763 2007-09-19 12:36:44Z btolman $
AM_CPPFLAGS = -I$(srcdir)/../../../src -I$(srcdir)/../../../lib/geomatics
# In this order!
LDADD = ../../../lib/geomatics/libgeomatics.la ../../../src/libgpstk.la

bin_PROGRAMS = mergeSRI tkalm

mergeSRI_SOURCES = mergeSRI.cpp  
tkalm_SOURCES = tkalm.cpp

