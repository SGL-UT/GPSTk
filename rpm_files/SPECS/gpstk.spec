%define name gpstk
%define version 5.0.0
%define release 1

Summary:        GPS Toolkit
Name:           %{name}
Version:        %{version}
Release:        %{release}
License:        LGPL
Source:         %{name}-master.tar.gz
URL:            https://github.com/SGL-UT/GPSTk
Group:          Development/Libraries
BuildRequires: cmake
BuildRequires: swig
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
cmake -DCMAKE_INSTALL_PREFIX=/ -DBUILD_EXT=ON -DBUILD_PYTHON=OFF -DBUILD_FOR_PACKAGE_SWITCH=ON ../
make all -j 4

# Install bin/lib/include folders in RPM BUILDROOT for packaging
%install
cd build
make install -j 4 DESTDIR=$RPM_BUILD_ROOT/usr
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
