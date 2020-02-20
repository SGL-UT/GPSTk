GPSTk 3.0.0 Release Notes
========================

 * Major version bump triggered by migration of MDH code from SGLTk into its own repository - MDHTk

Updates since v2.12.2
---------------------

**Build System and Test Suite**
 * Added conda recipes for compiling GPSTK conda packages.


**Gitlab CI**
 * Added job to the CI pipeline to verify that those recipes compile.


**Library Changes**
 * Added parityStatus flag to PackedNavBits. REASON: Needed to support proper conversion of raw receiver files to MDH.


Fixes since v2.12.2
--------------------
 * Fixed a bug that prevented some compiler arguments to be passed through build.sh


 Removed Code due to Deprecation
 -------------------------------
 * /apps
   * /clocktools
     *  rmoutlier
     *  dallandev
     *  ffp
     *  mallandev
     *  nallandev
     *  oallandev
     *  ohadamarddev
     *  ORDPhaseParser
     *  pff
     *  scale
     *  tallandev
     *  TIAPhaseParser
     *  trunc
     *  allanplot.py
   * /differential
     *  vecsol
   * /filetools
     *  GloDump
     *  rinexthin
   * /positioning
     *  posInterp
 * /lib
   * /Geodyn
     *  ASConstant.hpp
     *  AtmosphericDrag
     *  CiraExpotentialDrag
     *  EarthBody
     *  EarthOceanTide
     *  EarthPoleTide
     *  EarthSolidTide
     *  EGM96GravityModel
     *  EquationOfMotion.hpp
     *  ForceModel.hpp
     *  ForceModelList
     *  HarrisPriesterDrag
     *  IERS
     *  IERSConventions
     *  Integrator.hpp
     *  JGM3GravityModel
     *  KeplerOrbit
     *  MoonForce
     *  Msise00Drag
     *  PvtStore
     *  ReferenceFrames
     *  RelativityEffect
     *  RungeKuttaFehlberg
     *  SatOrbit
     *  SatOrbitPropogator
     *  SolarRadiationPressure
     *  Spacecraft
     *  SphericalHarmonicGravity
     *  SunForce
     *  UTCTime
