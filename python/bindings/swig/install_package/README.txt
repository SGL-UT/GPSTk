==============================
GPSTk Python Extension Package
==============================

Introduction:
-------------
The GPSTk python extension package provides access to the GPSTk C++ library 
from within python. It is built using SWIG and CMake, and installed with a 
standard setup.py script using the python distutils module.

Getting started:
----------------
Summary of the package files and directory structure:

* CHANGES.txt contains information on what has changed since the last release.
* INSTALL.txt contains detailed installation instructions.
* setup.py is included for use with distutil based installs, e.g. "$ python setup.py install"
* MANIFEST.in contains a list of non-source package files that will be installed with the package.
* LICENSE.txt, COPYING.txt, and COPYING.LESSER.txt contain information on code reuse and licensing.
* setup.cfg and included a hook for future development of a setuptools based install procedure.
* gpstk/ is the top-level directory of the python package and contains gpstk/data/ and gpstk/test/
* examples/ contains example scripts that demonstrate use of the package, e.g. examples/example1.py.

Package File Listing:
---------------------
This assumes that the root directory of the python package install has been
assigned to an environment variable PKG_INSTALL. For example,

    PKG_INSTALL=$GPSTK/python/bindings/swig/install_package

With that in mind, the package file manifest is as follows:

$PKG_INSTALL/README.txt
$PKG_INSTALL/INSTALL.txt
$PKG_INSTALL/LICENSE.txt
$PKG_INSTALL/CHANGES.txt
$PKG_INSTALL/MANIFEST.in
$PKG_INSTALL/setup.py
$PKG_INSTALL/gpstk
$PKG_INSTALL/gpstk/__init__.py
$PKG_INSTALL/gpstk/_gpstk.so
$PKG_INSTALL/gpstk/gpstk.py
$PKG_INSTALL/gpstk/test
$PKG_INSTALL/gpstk/test/__init__.py
$PKG_INSTALL/gpstk/test/test.py
$PKG_INSTALL/gpstk/test/test_gpstk_timeconvert.py
$PKG_INSTALL/gpstk/test/test_gpstk_util.py
$PKG_INSTALL/gpstk/test/test_gpstk_store.py
$PKG_INSTALL/gpstk/test/test_gpstk_time.py
$PKG_INSTALL/gpstk/test/gpstk_timeconvert.py
$PKG_INSTALL/gpstk/data
$PKG_INSTALL/gpstk/data/__init__.py
$PKG_INSTALL/gpstk/data/rinex2nav_data.txt
$PKG_INSTALL/gpstk/data/rinex2obs_data.txt
$PKG_INSTALL/gpstk/data/rinexmet_data.txt
$PKG_INSTALL/gpstk/data/sem_data.txt
$PKG_INSTALL/gpstk/data/sp3_data.txt
$PKG_INSTALL/gpstk/data/yuma_data.txt
$PKG_INSTALL/examples
$PKG_INSTALL/examples/__init__.py
$PKG_INSTALL/examples/example1.py
$PKG_INSTALL/examples/example2.py
$PKG_INSTALL/examples/example3.py
$PKG_INSTALL/examples/example4.py
$PKG_INSTALL/examples/example5.py
$PKG_INSTALL/examples/position_difference.py
$PKG_INSTALL/examples/sem_plot.py
