.. _build_label:


Building
=================

The build system is fairly complex, thanks to a large amount of auto-generated code procedures:

- Compiling object files for the C++ sources
- Parsing Doxygen comments in C++ source for SWIG to read later
- SWIG generating the gpstk wrap file (which is well over 100K lines) and the gpstk pyython wrap file
- Compiling and linking the wrapped source with the original object files
- Auto-generating the init files to set up the package structure


This example places the gpstk directory in the home folder and builds
to :file:`~/.local/lib/python2.7/site-packages/` , which is a source
that python will automatically search for packages on a linux system.

A Windows user would typically
use :file:`C:\\Python27\\Lib\\site-packages\\` instead.


First clone to git repo to your home directory (or wherever you want), then:

:command: `cd ~/gpstk/dev`

:command: `doxygen`

:command: `cd ~/gpstk/dev/apps/bindings/swig`

:command: `python docstring_generator.py`

:command: `mkdir bin`

:command: `cd bin`

:command: `cmake ..`

:command: `make`

:command: `python gpstk_builder.py  ~/.local/lib/python2.7/site-packages/`


You can also run gpstk_builder.py (the final command above) without any arguments to build to the system default.

On a debian-based linux distro, this will be :file:`/usr/local/lib/python2.7/dist-packages`

For example:

:command: `sudo python gpstk_builder.py`

Note that this would require root privileges.

Binary files
=======================
In the future, we plan to release binary installers that will include the Python bindings of the GPSTk,
but for the time being, it must be built from source.
