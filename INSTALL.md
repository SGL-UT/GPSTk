Installation of the GPSTk library and applications
--------------------------------------------------

Contents:
---------

* Introduction
* Dependencies
* UNIX-like Environment: Preparation
* UNIX-like Environment: Automated Build and Install
* UNIX-like Environment: Manual Build and Install
* UNIX-like Environment: Python Bindings Build and Install
* OSX and XCode: Building and Installing with CMake
* Windows and Microsoft Visual Studio: Building with CMake
* Linking Against the GPSTk Library

Introduction:
-------------

The most up-to-date project information can be found at the project website, 
including build and install instructions, the latest project news, a support 
forum, IRC, and contact information:

    http://www.gpstk.org/

The easiest way to build an install GPSTk is to use the shell script provided 
with the source distribution. See below for more details on use of the script, 
details on building on POSIX platforms, and where to get instructions for 
building with CMake, both with Xcode on OSX and Visual Studio on Windows.


Dependencies:
-------------

This project uses CMake for all of its configuration, build, install & testing tasks.
   
    http://www.cmake.org/

This project uses SWIG for generation of python bindings for the C++ library.
   
    http://www.swig.org/

This project (optionally) uses doxygen to dynamically generate API documentation.
   
    http://www.doxygen.org/

This project (optionally) uses Sphinx for some documentation generation
   
    http://sphinx-doc.org/


UNIX-like Environment: Preparation:
-----------------------------------

This section describes build and installation under the following environments:

   - Linux, OSX, Solaris

The following procedure will build and install the GPSTk.

   1. Ensure that prerequisites such as cmake and swig have been installed.

   2. Obtain the GPSTk source distribution.

   3. If needed, extract the GPSTk compressed archive.
      For example, using GNU tar and gunzip:

         $ tar -xvzf gpstk.tgz

   4. Navigate to the root directory of the extract GPSTk file tree, herein
      called $gpstk_root:

         $ cd $gpstk_root


UNIX-like Environment: Automated Build, Install, and Package:
-------------------------------------------------------------

If you prefer automation, run the GPSTk script. See the script help for details 
on those parameters:

    $ cd $gpstk_root

    $ ./build.sh -h

Note that the script will NOT update your shell environment, so you
will need to export the library install path if you install to a 
non-standard path. The build script will test your paths to and warn
you if your installs cannot be found.

Typical builds (no install) will look like this:

    $ ./build.sh -e

Typical user installs will look like this:

    $ export LD_LIBRARY_PATH=$HOME/.local/gpstk/lib:$LD_LIBRARY_PATH

    $ ./build.sh -eu

Typical test and install will look like this:

    $ export LD_LIBRARY_PATH=/tmp/test/lib:$LD_LIBRARY_PATH

    $ ./build.sh -et -i /tmp/test

Typical install and binary tarball package (in build folder) will look like this:

    $ export LD_LIBRARY_PATH=/tmp/test/lib:$LD_LIBRARY_PATH

    $ ./build.sh -ep -i /tmp/test


UNIX-like Environment: Manual Build, Install, and Package:
----------------------------------------------------------

If you instead prefer or need to run commands one at a time, run the following 
commands instead of build.sh:

   1. If you want documentation of the C++ classes and improved python 
      docstrings:

         $ cd $gpstk_root

         $ doxygen 

   2. Create a directory for your build:

         $ cd $gpstk_root

         $ mkdir $gpstk_root/build

   3. Change into the $gpstk_root/build directory, this will be where all of the
      build files will reside.

   4. If you prefer to build and install GPSTk as a system library in /usr/local, 
      execute the following commands:

         $ cd $gpstk_root/build

         $ cmake $gpstk_root

         $ make

         $ make install 

   5. If you prefer to build and install GPSTk as a user library in $HOME/.local, 
      skip the previous step, and instead execute the following commands:

         $ cd $gpstk_root/build

         $ cmake -DPYTHON_INSTALL_PREFIX=$HOME/.local -DCMAKE_INSTALL_PREFIX=$HOME/.local/gpstk -DBUILD_EXT=1 -DBUILD_PYTHON=1 $gpstk_root

         $ export LD_LIBRARY_PATH=$HOME/.local/gpstk/lib:$LD_LIBRARY_PATH

         $ make

         $ make install 

         $ export PATH=$HOME/.local/gpstk/bin:$PATH

   6. If you want to package the binaries into a tarball, execute the following commands:

         $ cd $gpstk_root/build

         $ cmake -DPYTHON_INSTALL_PREFIX=$HOME/.local -DCMAKE_INSTALL_PREFIX=$HOME/.local/gpstk $gpstk_root

         $ export LD_LIBRARY_PATH=$HOME/.local/gpstk/lib:$LD_LIBRARY_PATH

         $ make       

         $ make package


UNIX-like Environment: Python Bindings Build and Install:
---------------------------------------------------------

As the build.sh script builds the Python Bindings by default:

    $ cd $gpstk_root

    $ ./build.sh -h

Note that the script will NOT update your shell environment, so you
will need to export the library install path if you install to a 
non-standard path. The build script will test your paths to and warn
you if your installs cannot be found.

Typical system installs with python bindings will look like this:

    $ ./build.sh -e

Typical user installs with python bindings will look like this:

    $ ./build.sh -eu

Typical user installs with python bindings and documentation will look like this:

    $ ./build.sh -eud


OSX and XCode: Building and Installing with CMake:
--------------------------------------------------

Step-by-step procedure with pictures on building, installing, and testing the GPSTk under Xcode can be 
found at gpstk.org:

    http://www.gpstk.org/bin/view/Documentation/BuildingGPSTkUnderOSX


Windows and Microsoft Visual Studio: Building with CMake:
---------------------------------------------------------

Step-by-step procedure with pictures on building, installing, and testing the GPSTk under Visual Studio 
2012 can be found at gpstk.org:

    http://www.gpstk.org/bin/view/Documentation/BuildingGPSTkUnderWindows


Linking Against the GPSTk Library:
----------------------------------

If you are developing against your install of GPSTk, and have added
the relevant GPSTk library #include statements in you implementation file,
then the simplest example of building your application in a Bash script 
would look like this:

    INPUT=Your_App.cpp
    OUTPUT=Your_App.exe
    GPSTK_INSTALL=$HOME/.local/gpstk
    COMPILER="gcc -Wall"

    COMMAND_STRING="$COMPILER -I$GPSTK_INSTALL/include -L$GPSTK_INSTALL/lib -lgpstk -o $OUTPUT $INPUT"
    echo "$COMMAND_STRING"
    eval $COMMAND_STRING

