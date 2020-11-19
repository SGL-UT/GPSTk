%define name gpstk
%define version 8.0.0
%define release 1

Summary:        GPS Toolkit
Name:           %{name}
Version:        %{version}
Release:        %{release}%{?dist}
License:        LGPL
Source:         %{name}-master.tar.gz
URL:            https://github.com/SGL-UT/GPSTk
Group:          Development/Libraries
BuildRequires: cmake
BuildRequires: gcc
BuildRequires: gcc-c++
BuildRequires: ncurses-devel

%description
The GPS Toolkit (GPSTk) is an open-source (LGPL) project sponsored by
the Space and Geophysics Laboratory (SGL), part of the Applied Research
Laboratories (ARL) at The University of Texas at Austin.
The primary goals of the GPSTk project are to:
* provide applications for use by the GNSS and satellite navigation community.
* provide a core library to facilitate the development of GNSS applications.

%prep
%setup -n %{name}-master

# Setup and build GPSTk utilizing CMake
%build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$RPM_BUILD_ROOT/usr -DBUILD_EXT=ON -DUSE_RPATH=OFF -DBUILD_FOR_PACKAGE_SWITCH=ON ../
make all -j 4

# Install bin/lib/include folders in RPM BUILDROOT for packaging
%install
cd build
make install -j 4
rm -rf $RPM_BUILD_ROOT/usr/README.md

%clean
rm -rf $RPM_BUILD_ROOT

# Specify files and folders to be packaged
%files
%defattr(-,root,root)
%doc README.md COPYING.md RELNOTES.md
/usr/include/gpstk
/usr/bin/*
/usr/lib64/*
/usr/share/cmake/GPSTK


%changelog
* Thu Nov 19 2020 David Barber <dbarber@arlut.utexas.edu>
- Updated for v8.0.0 release
- Update IonoModel to include all modernized GPS bands.
- Update SVNumXRef Add SVN77/PRN14
- Fix OrbSysGpsL_56 Correct iono parameter units
- Update CI to verify proper copyright and license header.
- Refactor pipeline to use git https protocol instead of ssh
- Update copyright language to reflect year 2020
- Fix EL8 RPM Generation to avoid including build-id files.
- Add Centos8 build/test/package/deploy jobs to the CI pieline.
- Refactor `PRSolution2` to `PRSolutionLegacy`.
- Update CODEOWNERS file.
- Fix SP3EphemerisStore to properly handle correlation data from SP3c files.
- Refactor calculate_ord method definition out of header file.
- Update python 2.7 conda recipe to avoid using preprocessig-selector for enum34.
- Update Nav reader code to properly assign being/end validity values for non-GPS GNSS data.
* Fri Sep 18 2020 Andrew Kuck <kuck@arlut.utexas.edu>
- Updated for v7.0.0 release
- Fix GalEphemeris Corrected behavior of isHealthy()
- Update SWIG Bindings to wrap enumerations more correctly.
- Update OceanLoadTides.hpp Add reference to SPOTL in doxygen comments
- Refactor gpstk pipeline downstream jobs
- Refactor debian dpkg-buildpackage Update control files to remove python bindings
- Refactor debian8 remove python bindings
- Dropping Python Swig binding support for RHEL7/DEB7
- Refactor ObsID and RinexObsID initialization of containers (tcDesc, etc.) to use C++11 syntax and eliminate the use of a singleton initializer.
- Move RINEX-isms (such as the string constructor for decoding RINEX obs IDs) in ObsID into RinexObsID where they belong.
- Refactor swig bindings for enums to use similar naming conventions between C++ and python (e.g. gpstk::TrackingCode::CA in C++ and gpstk.TrackingCode.CA in python)
- Add EnumIterator class to provide the ability to iterate over the above enum classes.
- Rename TrackingCode enumerations to better support codes that RINEX does not by using names based on the ICDs rather than what RINEX uses.
- Refactor enumerations in TimeSystem, ReferenceFrame, IERSConvention, SatID, NavID and ObsID (SatelliteSystem, NavType, ObservationType, CarrierBand, TrackingCode) to use strongly typed enumerations and move them outside the scope of those classes.
- Add SatMetaDataStore findSatByFdmaSlot
- Add Documentation Artifact to CI Pipeline
- Refactor Yuma/SEM file support back into ext
- Fix core/lib include statements to search GPSTk include directory.
- Fix OrbSysGpsL_55 Restore output of text message
- Fix various pieces of code to resolve warning messages on various platforms.
- Add CODEOWNERS file.
* Mon Sep 14 2020 Andrew Kuck <kuck@arlut.utexas.edu>
- Updated for v7.0.0 release
- Update OceanLoadTides.hpp Add reference to SPOTL in doxygen comments
- Refactor gpstk pipeline downstream jobs
- Refactor debian dpkg-buildpackage Update control files to remove python bindings
- Refactor debian8 remove python bindings
- Dropping Python Swig binding support for RHEL7/DEB7
- Refactor ObsID and RinexObsID initialization of containers (tcDesc, etc.) to use C++11 syntax and eliminate the use of a singleton initializer.
- Move RINEX-isms (such as the string constructor for decoding RINEX obs IDs) in ObsID into RinexObsID where they belong.
- Refactor swig bindings for enums to use similar naming conventions between C++ and python (e.g. gpstk::TrackingCode::CA in C++ and gpstk.TrackingCode.CA in python)
- Add EnumIterator class to provide the ability to iterate over the above enum classes.
- Rename TrackingCode enumerations to better support codes that RINEX does not by using names based on the ICDs rather than what RINEX uses.
- Refactor enumerations in TimeSystem, ReferenceFrame, IERSConvention, SatID, NavID and ObsID (SatelliteSystem, NavType, ObservationType, CarrierBand, TrackingCode) to use strongly typed enumerations and move them outside the scope of those classes.
- Add SatMetaDataStore findSatByFdmaSlot
- Add Documentation Artifact to CI Pipeline
- Refactor Yuma/SEM file support back into ext
- Fix core/lib include statements to search GPSTk include directory.
- Fix OrbSysGpsL_55 Restore output of text message
- Fix various pieces of code to resolve warning messages on various platforms.
- Add CODEOWNERS file.
* Thu Jul 30 2020 Andrew Kuck <kuck@arlut.utexas.edu>
- Updated for v6.0.0 release
- Update SWIG bindings to fix memory leaks for Matrix.
- Fix YDSTime to eliminate use of uninitialized variables.
- Fix FFBinaryStraem to eliminate use of uninitialized variables.
- Fix TimeRange_T to eliminate dereferencing of unallocated memory pointers.
- Fix OrbElem to eliminate use of uninitialized variables.
- Fix BDSEphemeris to eliminate use of uninitialized variables.
- Fix GlobalTropModel to eliminate use of uninitialized variables.
- Fix RinexObsHeader to eliminate use of uninitialized variables.
- Fix gdc::getArcStats to eliminate a memory leak.
- Fix WindowFilter to eliminate use of uninitialized variables.
- Fix OrbSysStore to eliminate a memory leak.
- Fix OrbAlmStore to eliminate a memory leak.
- Fix rstats to eliminate a memory leak.
- Fix dfix to eliminate use of uninitialized variables.
- Fix CNavFilter_T to eliminate a memory leak.
- Fix CNav2Filter_T to eliminate a memory leak.
- Fix Rinex3Obs_FromScratch_t.cpp to eliminate a memory leak.
- Fix CommandOption_T to eliminate a memory leak.
- Fix BasicFrameworkHelp_T to eliminate a memory leak.
- Fix RationalizeRinexNav to eliminate a memory leak.
- Fix OrbitEph to initialize all data members.
- Fix RinDump to eliminate a memory leak.
- Fix FFTextStream to eliminate an invalid memory read.
- Fix PRSolve to eliminate a memory leak.
- Fix rinheaddiff to eliminate a memory leak.
- Fix RinDump to eliminate a memory leak.
- Fix AntennaStore BeiDou phase center
- Update CI Pipeline to be more adept at producing downloadable packages.
- Update and Adding swig support for IonoModel and Nav Reader
- Update test data to use fortran format numbers in RINEX nav data.
- Refactor text stream classes to use stream operations instead of string construction.
- Update existing uses of doub2for etc. to use FormattedDouble.
- Deprecate doub2sci, doubleToScientific, sci2for, doub2for, and for2doub in StringUtils.
- Add FormattedDouble as a more versatile replacement for doub2for, for2doub, etc.
- Update gitlab-ci.yml to declare a registry associated with each image.
- Update SVNumXRef Add SVN76/PRN23
- Refactor SatMetaDataStore to split records to reduce redundancy of data, particularly of data that does not change over the life of the satellite.
- Update SVNumXRef Deassign PRN23 from SVN60
- Deprecate FileHunter class.
- Update AntexData Add BeiDou-3, QZSS, NAVIC satellite types
- Fix FileSpecFind to support both forward slash and backslash file separators under windows.
- Add Filter class to FileSpecFind to support matching of sets of values.
- Fix FileSpecFind to handle more use cases under windows.
- Add FileSpec methods hasTimeField and hasNonTimeField.
- Update FileSpec for a more optimal implementation of hasField
- Update FileFilterFrameWithHeader to use FileSpecFind instead of FileHunter.
- Update FileFilterFrame to use FileSpecFind instead of FileHunter.
- Add CI testing on Redhat 7 with SWIG 3 installed.
- Update CommonTime::setTimeSystem to return a reference to itself.
- Update swig CMakeLists.txt to preclude setting RUNPATH in _gpstk.so thus preventing it from pointing to the wrong libgpstk.so
- Fix SWIG vector constructor to take a list of integers
- Update Ashtech and other classes to prevent SWIG from generating bad code.
- Add Rinex3ObsHeader accessors for wavelengthFactor in support of python.
* Wed Jun 17 2020 Andrew Kuck <kuck@arlut.utexas.edu>
- Updated for v5.0.1 release
- Fix Swig wrapper of PRSolution to allow passing list of GNSS system.
* Fri Jun 12 2020 Andrew Kuck <kuck@arlut.utexas.edu>
- Updated for v5.0.0 release
- Note: Rinex 3.04 support is not fully in compliance with "9.1 Phase Cycle Shifts", although that does not affect any of our use cases.
- Update PreciseRange to input frequencies as well as GNSS.
- Update PRSolution for multi-GNSS solution, including one Rx clock per system.
- Update OrbitEph::adjustValidity to subtract two hours only for GPS.
- Add RinexObsID support for decoding channel number and ionospheric delay pseudo-observables.
- Add ObsID support for numerous previously unsupported codes.
- Add FreqConsts.hpp to replace the deprecated constants with new names based on ICD naming instead of RINEX.
- Remove frequency/wavelength constants from GNSSconstants.hpp and put them in DeprecatedConsts.hpp
- Update Rinex3ObsData to support channel number and ionospheric delay data.
- Fix QZSEphemeris to use a sane begin time for QZSS nav.
- Fix Rinex3NavData to use a sane begin time for QZSS nav.
- Fix Rinex3ClockData/Rinex3ClockHeader to decode systems using existing classes.
- Update PRSolve to input multiple GNSS options, and for multi-GNSS solution, including one Rx clock per system.
- Update RinSum to improve support aux headers over simple comment support.
- Update RinDump to support all current (as of RINEX 3.04) codes
- Add FileUtils/FileUtils_T comments explaining debian 7 test failures.
- Move a collection of tests and code from ext to core.
- Fix SWIG bindings to use Python-list to C-array typemaps in gpstk_typemaps.i
- Update RinSum to have better diagnostics when there is an invalid Rinex header
- Add BasicFramework documentation.
- Update CommandOption classes to return a const reference instead of a copy of a vector.
- Update Position::transformTo,asGeodetic,asECEF to return a reference.
- Fix Rinex3ObsHeader::Fields SWIG bindings.
- Fix Rinex3ObsHeader::Fields SWIG bindings.
- Add SWIG bindings to support the nested classes within Rinex3ObsHeader.
- Fix RINEX reference data for tests containing the "Signal Strenth" typo, or remove it for RINEX 2 tests.
- Update Rinex3ObsHeader to use a nested class called "Flags" for header field management, replacing the bit field.
- Modify installation script to create site-packages if it doesn't exist.
- Update Yuma_T test to use the most modern cmake test script and df_diff to account for minor differences in floating point least significant digits.
- Update rstats testing to use portable ctest mechanisms instead of the python script, and split the truth data accordingly.
- Remove FileHunter_T failing test from Windows build in preparation for future deprecation of the FileHunter class.
- Update FileSpec to use WIN32 precompiler macro instead of _WIN32 to get the correct file separator.
- Update CommandOptionParser to remove the .exe file extension when printing help under Windows.
- Fix RinexSatID/SP3SatID to properly handle QZSS and GEO satellite IDs over 100
- Add FileSpecFind as an eventual replacement for FileHunter
- Deprecate SVNumXRef in favor of SatMetaDataStore
- Update MJD/JD timeconvert tests to be able to pass under windows.
- Update CI to retain, as CI artifacts, output from failed tests.
- Fix decimation bug Rinex3ObsLoader corrected decimation logic
- Update OrbitEphStore to have more information when a potentially corrupt ephemeris is added.
- Add NavID Add methods to convert between enumeration and string and test them.
- Add ObsID Add methods to convert between enumeration and string and test them.
- Update SWIG files so that gpstk::Exception is treated as a Python Exception
- Add tests for computeXvt and getSVHealth with unhealthy satellites
* Mon Mar 30 2020 Andrew Kuck <kuck@arlut.utexas.edu>
- Updated for v4.0.0 release
- Update CMAKE to optimize at level O3 on linux by default.
- Updated call signatures in OrbElemRinex to support downstream callers.
- Add string conversions to-from SatID.SatelliteSystem
- Remove deprecated dynamic exception specifications.
- Update SVNumXRef to move PRN 18 from SVN 34 to SVN 75
- Add CI checks to ensure continued compatibility with Debian 7.
- Fix test failures observed running on Debian 10.
- Fix misleading comment on CommonTime.m_day
- Change OrbitEph to use CGCS2000 ellipsoid for BeiDou when computing relativity correction
- Fix BDSEphemeris to use GEO equations only above 7 degrees.
- Fix AlmOrbit to add missing final value of G.
- Fix BrcKeplerOrbit to use relativity correction and initial URE value of 0.
- Update svXvt to set Xvt.health when available.
- Fix slight errors in svXvt velocity computation.
- Modify RPM packages to seperate python installation from C++ apps/libs.
- Fix bug in timeconvert app breaking after year 2038
- Fix incorrect interval in output header from RinEdit if thinning option is used.
- Fix memory leak during OrbElmStore deallocation.
* Tue Mar 03 2020 Andrew Kuck <kuck@arlut.utexas.edu>
- Split single RPM into core and python packages for v3.1.0 release 2
* Tue Mar 03 2020 Andrew Kuck <kuck@arlut.utexas.edu>
- Updated for v3.1.0 release
* Tue Feb 11 2020 Bryan Parsons <bparsons@arlut.utexas.edu>
- Updated for v3.0.0 release
* Wed Jan 15 2020 Bryan Parsons <bparsons@arlut.utexas.edu>
- Updated for v2.12.2 release
* Wed Oct 9 2019 Bryan Parsons <bparsons@arlut.utexas.edu>
- Updated for v2.12.1 release
* Tue Sep 17 2019 Bryan Parsons <bparsons@arlut.utexas.edu>
- Update to build from master on CI
* Fri Aug 23 2019 Bryan Parsons <bparsons@arlut.utexas.edu>
- Update to add /usr/share directory to package
* Thu Aug 15 2019 Bryan Parsons <bparsons@arlut.utexas.edu>
- Updated for v2.12 release
* Tue Aug 13 2019 Bryan Parsons <bparsons@arlut.utexas.edu>
- Updated for gitlab ci rpm building
* Fri Apr 5 2019 Bryan Parsons <bparsons@arlut.utexas.edu>
- Updated for v2.11.2 release
* Mon Feb 4 2019 Bryan Parsons <bparsons@arlut.utexas.edu>
- Updated for v2.11.1 release
* Thu Jan 3 2019 Bryan Parsons <bparsons@arlut.utexas.edu>
- Updated for v2.11.0 release
* Tue Dec 11 2018 Bryan Parsons <bparsons@arlut.utexas.edu>
- Updated for v2.10.7 release
* Fri Oct 12 2018 Bryan Parsons <bparsons@arlut.utexas.edu>
- Updated for v2.10.6 release - second release
* Mon Sep 24 2018 Bryan Parsons <bparsons@arlut.utexas.edu>
- Updated for v2.10.5 release
* Mon Aug 27 2018 Bryan Parsons <bparsons@arlut.utexas.edu>
- Updated for v2.10.5 release
* Tue May 8 2018 Bryan Parsons <bparsons@arlut.utexas.edu>
- adding initial RPM configuration files

[ Some changelog entries trimmed for brevity.  -Editor. ]
