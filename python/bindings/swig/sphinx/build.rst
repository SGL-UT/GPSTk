.. _build_label:

Installation
======================

Preliminary steps
********************************************
Prerequisits for building and installing:

- install of cmake
- install of swig
- install of C++ compiler (e.g. gcc)
- install of python

Installing Pre-compiled Binaries
********************************************
At this point in time, pre-compiled binaries are not distributed.
Presently, the only option is to build from source.
In the future, we hope to release pre-built binaries.

Build and Install on POSIX using Bash script
******************************************************

The build system is composed of the following steps:

- Compiling object files for the C++ sources to generate a libgpstk.so
- Parsing Doxygen comments in C++ source for use by SWIG in creating docstrings
- SWIG generates C++ wrapper source code to map C++ types to python types
- Compiling and linking the C++ wrapper to generate a python extension module
- Moving files into the install package file tree to complet the python package

An example source file tree is provided here for the purpose
of giving context to the build and install examples below, listing paths 
and files that play critical roles in the build and install process: ::
  
	$GPSTK/script_gpstk.sh
	$GPSTK/dev/CMakeLists.txt
	$GPSTK/python/bindings/swig/CMakeLists.txt
	$GPSTK/python/bindings/swig/install_package/setup.py

Current build and install is automated for Linux only.
A bash script script_gpstk.sh, is included with this GPSTk 
distribution for this automation, but also to document the 
manual steps that support build and install on a wide 
variety of other platforms. The major dependencies are
CMake, SWIG, gcc, and python (distutils). See setup.py
for a detailed dependency listing with version numbers
that were tested.

Review the help comments of the build script found in the
top-level directory of the GPSTk file tree: ::
  
	$ script_gpstk.sh -h
  
The python bindings link against the C++ library, e.g.
libgpstk.so on Linux, so you must compile and install that library first.
To build and install the C++ library, you can use the included
bash script: ::
  
	$ script_gpstk.sh -c

Default install path of the C++ library shared object file is as follows: ::
  
	$GPSTK/install/libgpstk.so
  
Once installed, you must manually add the install path in 
$PATH or $LD_LIBRARY_PATH. For example: ::
  
	$ export LD_LIBRARY_PATH=$GPSTK/dev/install/lib

This is left for the user to do as we did not want the automated
script to change your environment variables without you knowing.

To generate and build the python wrapper code, run the bash script
found in the GPSTk top-level directory: ::
  
	$ script_gpstk.sh -p

Default install path for the python package is: ::

	$HOME/.local/lib/python<X.X>/site-packages/gpstk

To install the python package in an alternate path, use disutils and
the included setup.py script, for example: ::
  
    # Move to the install package root diretory
	$ cd $GPSTK/python/bindings/swig/install_package
	# run a check on the setup.py file
	$ python setup.py check
	# install the package
	$ python setup.py install
	
Alternatively, to force install into your user home file tree: ::

    # install the package under your home directory
    $ python setup.py --prefix=~/.local

Note that the CMake install uses setup.py.in to allow for CMake variable
values to be written into the final setup.py file which is then used with 
distutils based install. Though setup.py is regenerated from setup.py.in
by CMake, we've preserved setup.py for those who may not have CMake 
installed.

Additionally, the html documentation can be built/viewed with sphinx by: ::

    cd $GPSTK/python/bindings/swig/sphinx
    make html
    firefox _build/html/index.html
