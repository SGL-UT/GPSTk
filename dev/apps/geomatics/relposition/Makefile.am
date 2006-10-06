INCLUDES = -I../../../src -I../lib
LDADD = ./libbase.la ../lib/libgeomatics.la ../../../src/libgpstk.la

noinst_LTLIBRARIES = libbase.la
libbase_la_SOURCES = DataStructures.cpp CommandInput.cpp Configure.cpp ReadRawData.cpp ReadObsFiles.cpp EphemerisImprovement.cpp ClockModel.cpp Synchronization.cpp DoubleDifference.cpp EditDDs.cpp DataOutput.cpp Estimation.cpp Timetable.cpp ElevationMask.cpp ProcessRawData.cpp ComputeRAIMSolution.cpp EditRawDataBuffers.cpp

noinst_PROGRAMS = DDBase ddmerge 

DDBase_SOURCES = DDBase.cpp
ddmerge_SOURCES = ddmerge.cpp
