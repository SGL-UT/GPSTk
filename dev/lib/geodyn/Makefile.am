# $Id: Makefile.am 1331 2008-07-30 04:51:40Z ckuethe $
# please add new entries in alphabetic order
AM_CPPFLAGS = -I$(srcdir)/../../src -I$(srcdir)/../geomatics
lib_LTLIBRARIES = libgeodyn.la
libgeodyn_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
libgeodyn_la_SOURCES = AtmosphericDrag.cpp \
      CiraExponentialDrag.cpp \
	  EarthBody.cpp \
	  EarthOceanTide.cpp \
	  EarthPoleTide.cpp \
	  EarthSolidTide.cpp \
	  EGM96GravityModel.cpp \
	  ForceModelList.cpp \
	  HarrisPriesterDrag.cpp \
	  IERS.cpp \
	  IERSConventions.cpp \
	  JGM3GravityModel.cpp \
	  KeplerOrbit.cpp \
	  MoonForce.cpp \
	  Msise00Drag.cpp \
	  PvtStore.cpp \
	  ReferenceFrames.cpp \
	  RelativityEffect.cpp \
	  RungeKuttaFehlberg.cpp \
	  SatOrbit.cpp \
	  SatOrbitPropagator.cpp \
	  SolarRadiationPressure.cpp \
	  Spacecraft.cpp \
	  SphericalHarmonicGravity.cpp \
	  SunForce.cpp \
	  UTCTime.cpp

incldir = $(includedir)/gpstk
incl_HEADERS = ASConstant.hpp \
      AtmosphericDrag.hpp \
	  CiraExponentialDrag.hpp \
	  EarthBody.hpp \
	  EarthOceanTide.hpp \
	  EarthPoleTide.hpp \
	  EarthSolidTide.hpp \
	  EGM96GravityModel.hpp \
	  EquationOfMotion.hpp \
	  ForceModel.hpp \
	  ForceModelList.hpp \
	  HarrisPriesterDrag.hpp \
	  IERS.hpp \
	  IERSConventions.hpp \
	  Integrator.hpp \
	  JGM3GravityModel.hpp \
	  KeplerOrbit.hpp \
	  MoonForce.hpp \
	  Msise00Drag.hpp \
	  PvtStore.hpp \
	  ReferenceFrames.hpp \
	  RelativityEffect.hpp \
	  RungeKuttaFehlberg.hpp \
	  SatOrbit.hpp \
	  SatOrbitPropagator.hpp \
	  SolarRadiationPressure.hpp \
	  Spacecraft.hpp \
	  SphericalHarmonicGravity.hpp \
	  SunForce.hpp UTCTime.hpp

