%define name gpstk
%define version 4.0.0
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
