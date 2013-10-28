.. _build_label:

Installation
======================
There are two ways to build and install the GPSTk python bindings on your system:

1. Install using pre-compiled binaries. This is the easiest option for most users.

2. Download an archive file and run the setup.py script. This requires a C++ compiler.

3. Clone the git repo and use CMake and your system's local build tool. This requires CMake, SWIG and a C++ compiler.
   To generate the docstrings, this also requires Doxygen and you must also run several included python scripts.
   Developers of the GPSTk and users that wish to be on the bleeding edge should use this option.


Installing Using Binaries
********************************************
At this point in time, no pre-compiled binaries are provided yet.


Installing with setup.py [user build]
********************************************************

First download extract the source archive at TODO:ADD_LINK , the run one of the following commands:

For a Unix-like system that installs only for the current user: ::

    python setup.py install --prefix=~/.local

For a Unix-like system that installs for the system: ::

    sudo python setup.py install

For a Windows system using Python 2.7: ::

    setup.py install



Installing with CMake [developer build]
******************************************************

The build system is fairly complex, thanks to a large amount of auto-generated code procedures:

- Compiling object files for the C++ sources
- Parsing Doxygen comments in C++ source for SWIG to read later
- SWIG generating the gpstk wrap file (which is well over 600K lines) and the gpstk python wrap file
- Compiling and linking the wrapped source with the original object files
- Auto-generating the init files to set up the package structure


This example places the gpstk directory in the home folder and builds
to :file:`~/.local/lib/python2.7/site-packages/` , which is a source
that python will automatically search for packages on a Linux system.

A Windows user would typically
use :file:`C:\\Python27\\Lib\\site-packages\\` instead.


First clone to git repo to your home directory (or wherever you want), then: ::

    cd ~/gpstk/dev
    doxygen
    cd ~/gpstk/dev/apps/bindings/swig
    python docstring_generator.py
    mkdir bin
    cd bin
    cmake ..
    make
    python gpstk_builder.py  ~/.local/lib/python2.7/site-packages/

.. note:
        :file:`gpstk/dev/bindings_intaller/devinstall.sh` runs these commands


You can also run gpstk_builder.py (the final command above) without any arguments to build to the system default.

On a Debian-based Linux distro, this will be :file:`/usr/local/lib/python2.7/dist-packages`

For example: ::

    sudo python gpstk_builder.py

Note that this would require root privileges.


Additionally, the html documentation can be built/viewed with sphinx by: ::

    cd ~/gpstk/dev/apps/bindings/swig/sphinx
    make html
    firefox _build/html/index.html
