# $Id$
INCLUDES = -I$(srcdir)/../../../src -I$(srcdir)/../lib

LDADD = ../lib/libgeomatic.la ../../../src/libgpstk.la
DDBase_LDADD = ../lib/libgeomatic.la ../../../src/libgpstk.la ./libbase.la

lib_LTLIBRARIES = libbase.la
libbase_la_SOURCES = ClockModel.cpp CommandInput.cpp ComputeRAIMSolution.cpp \
Configure.cpp DataOutput.cpp DataStructures.cpp DoubleDifference.cpp \
EditDDs.cpp EditRawDataBuffers.cpp ElevationMask.cpp EphemerisImprovement.cpp \
Estimation.cpp ProcessRawData.cpp ReadObsFiles.cpp ReadRawData.cpp \
Synchronization.cpp Timetable.cpp

bin_PROGRAMS = DDBase ddmerge 

DDBase_SOURCES = DDBase.cpp
ddmerge_SOURCES = ddmerge.cpp
