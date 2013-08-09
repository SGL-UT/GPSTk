.. _build_label:

Installation
======================
There are two ways to build and install the GPSTk python bindings on your system:

1. Download an archive file and run the setup.py script. This only requires a C++ compiler. Most users should use this, there is no
   loss in efficiency in this option, it only eliminates some system-independent parsing steps.
2. Clone the git repo and use CMake and your system's local build tool. This requires SWIG and a C++ compiler. To generate
   the docstrings, this also requires Doxygen and you must also run several included python scripts.
   Developers of the GPSTk and users that wish to be on the bleeding edge might prefer this option.


Building with setup.py [user build/install]
********************************************************

Extract the archive somewhere. Open a terminal, change into the directory that was extracted and run: ::

    python setup.py install

On a Unix system you will likely need root so use: ::

    sudo python setup.py install

If no root is available you can do a user local install with: ::

    python setup.py install --prefix=~/.local


.. note:: gpstk/apps/bindings_installer/swig/package.sh runs the developer build system
          below and produces the files used by distutils in setup.py. This is used by developers
          to package the files and create archives/binaries more easily.


Building with CMake [developer build/install]
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

If you frequently rebuild it, it might be a good idea to but the last few (usually the last 3) commands above in a bash alias/function/script.

You can also run gpstk_builder.py (the final command above) without any arguments to build to the system default.

On a Debian-based Linux distro, this will be :file:`/usr/local/lib/python2.7/dist-packages`

For example: ::

    sudo python gpstk_builder.py

Note that this would require root privileges.


Additionally, the html documentation can be built/viewed with sphinx by: ::

    cd ~/gpstk/dev/apps/bindings/swig/sphinx
    make html
    firefox _build/html/index.html

Binary files
*************************
In the future, we plan to release binary installers that will include the Python bindings of the GPSTk,
but for the time being, it must be built from source.
