.. _devdocs_label:

Developer Documentation
=========================================

The information herein is meant to help developers and maintainers of the GPSTk.
It contains essential build and release information as well as general 
information about the organization of the source code file tree to help others 
contribute to it.


Source Code Organization
****************************************

The majority of the SWIG interface files and Python source files for the C++ 
bindings are contained in the following path: ::

    $GPSTK/swig

Contents of the $GPSTK/swig directory:

- :file:`../CMakeLists.txt` is the CMake file for use in generating the build and install targets for the python package
- :file:`../gpstk.i` is the primary interface file for SWIG which contains include directives for other files.i
- :file:`../src/` is for additional SWIG interface files.i, all of which are included in :file:`gpstk.i`
- :file:`../sphinx/` contains all resources for the external documentation.
- :file:`../install_package/` contains the python package that will be installed.

Packaging System
*****************************************

The shell script :file:`../build.sh` does much of the packaging work, it:

- runs the the doxygen -> docstring scripts
- creates a sdist from setup.py in tar.gz and zip formats
- builds the documentation files
- creates a 64-bit debian binary

The following files are output:

- :file:`./dist/gpstk-2.{x}.tar.gz` [tar source archive to run setup.py on]
- :file:`./dist/gpstk-2.{x}.zip` [zip source archive to run setup.py on]
- :file:`./dist/gpstkpythondoc.zip` [Sphinx html documentation to go on PyPI]
- :file:`./dist/deb_dist/python-gpstk_2.x-1_amd64.deb` [64-bit Debian binary]

.. note::
    The python package stdeb is used by the script to generate the debian. It can be found on PyPI: stdeb_.

.. _stdeb: https://pypi.python.org/pypi/stdeb



Uploading
*****************************************

1. Run the above packaging script
2. Upload the sdist to PyPI: ::

        python setup.py sdist upload

3. Login to your account on PyPI: https://pypi.python.org/pypi
4. Upload the documentation archive gpstk/python/bindings/dist/gpstkpythondoc.zip
