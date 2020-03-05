%define name gpstk
%define version 3.1.0
%define release 2

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
