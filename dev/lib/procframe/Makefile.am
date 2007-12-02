# $Id: $

# please add in alphabetic order
INCLUDES = -I$(srcdir)/../../src/
lib_LTLIBRARIES = libprocframe.la
libprocframe_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
libprocframe_la_SOURCES = CodeSmoother.cpp ComputeIURAWeights.cpp \
ComputeMOPSWeights.cpp ComputeWindUp.cpp DeltaOp.cpp LICSDetector.cpp \
ModelObs.cpp ModelObsFixedStation.cpp MWCSDetector.cpp NablaOp.cpp \
OneFreqCSDetector.cpp PCSmoother.cpp ProcessingVector.cpp SimpleFilter.cpp \
SolverLMS.cpp SolverWMS.cpp XYZ2NED.cpp XYZ2NEU.cpp

incldir = $(includedir)/gpstk
incl_HEADERS = CodeSmoother.hpp ComputeIURAWeights.hpp ComputeMOPSWeights.hpp \
ComputeWindUp.hpp DeltaOp.hpp LICSDetector.hpp ModelObs.hpp \
ModelObsFixedStation.hpp MWCSDetector.hpp NablaOp.hpp OneFreqCSDetector.hpp \
PCSmoother.hpp ProcessingVector.hpp SimpleFilter.hpp SolverLMS.hpp \
SolverWMS.hpp XYZ2NED.hpp XYZ2NEU.hpp
