==============================
GPSTk Python Extension Package
==============================

-------------
Introduction:
-------------

The GPSTk python extension package provides access to the GPSTk C++ library from within python.

---------------------
Package File Listing:
---------------------

README.txt
INSTALL.txt
LICENSE.txt
CHANGES.txt
MANIFEST.txt
setup.py
	
/gpstk
/gpstk/__init__.py
/gpstk/_gpstk.so
/gpstk/gpstk.py

/gpstk/test
/gpstk/test/__init__.py
/gpstk/test/test.py
/gpstk/test/test_gpstk_timeconvert.py
/gpstk/test/test_gpstk_util.py
/gpstk/test/test_gpstk_store.py
/gpstk/test/test_gpstk_time.py
/gpstk/test/gpstk_timeconvert.py

/gpstk/data
/gpstk/data/__init__.py
/gpstk/data/rinex3nav_data.txt
/gpstk/data/rinex3obs_data.txt
/gpstk/data/rinex3obs_data.txt.new
/gpstk/data/rinexmet_data.txt
/gpstk/data/sem_data.txt
/gpstk/data/sp3_data.txt
/gpstk/data/yuma_data.txt

/examples
/examples/__init__.py
/examples/example1.py
/examples/example2.py
/examples/example3.py
/examples/example4.py
/examples/example5.py
/examples/position_difference.py
/examples/sem_plot.py

--------------------------
Package File Descriptions:
--------------------------

README.txt
- This file.

 INSTALL.txt
 - Instructions on how to install this package.
 - See also setup.py and setup.cfg
 
LICENSE.txt
- Details of use and re-use of the GPSTk source code
- see also COPYING.txt and COPYING.LESSER.txt
 
CHANGES.txt
- Notable changes since the last distribution version.

MANIFEST.txt
- List of non-source package files that will be installed with the package.

setup.py
- used to install this package
- see also INSTALL.txt
