# $Id:$
INCLUDES = -I$(srcdir)/../../src
LDADD = ./libsimlib.la ../../src/libgpstk.la 

lib_LTLIBRARIES = libsimlib.la 
libsimlib_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
libsimlib_la_SOURCES = normal.cpp CCReplica.cpp EMLTracker.cpp \
IQStream.cpp NavFramer.cpp ;


bin_PROGRAMS = codeDump corltr gpsSim iqdump simpleNav tracker  

codeDump_SOURCES = codeDump.cpp
corltr_SOURCES = corltr.cpp
gpsSim_SOURCES = gpsSim.cpp
iqdump_SOURCES = iqdump.cpp
simpleNav_SOURCES = simpleNav.cpp
tracker_SOURCES = tracker.cpp

