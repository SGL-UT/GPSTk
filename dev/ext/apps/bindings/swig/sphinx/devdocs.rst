.. _devdocs_label:

Developer Documentation
=========================================

The information on this page is meant to help developers and maintainers for the GPSTk.
It contains essential build/release information as well as general information about the
layout of the source code to help others add to it.



Source Code Organization
****************************************

Most SWIG/Python code for the bindings is contained within gpstk/dev/apps/bindings/swig.

Exceptions to this are:

- :file:`dev/setup.py` is the setup file used by users
- :file:`dev/MANIFEST.in` is the MANIFEST file helper distutils uses to know what to package into a source distribution
- :file:`dev/apps/bindings_installer/packager.sh` see `Packaging System` below

Within the main folder we have:

- :file:`gpstk.i` is the "master" interface file for SWIG, all other .i files are included by it
- :file:`doc/` is for auto-generated documentation files created by :file:`doc.py` with :file:`doxy2swig.py`
- :file:`src/` is for extra .i files used to declare interfaces, these are the files included in :file:`gpstk.i`
- :file:`tests/` has all python binding test code in it. :file:`tests/test.py` runs all other test scripts
- :file:`examples/` contains some examples, including ports of the original C++ examples and apps
- :file:`bin/gpstk_builder.py` is a helper script that handles moving the output files and creating the __init__.py files
- :file:`sphinx/` contains all resources for the external documentation.


Packaging System
*****************************************

The shell script :file:`gpstk/dev/apps/bindings_installer/package.sh` does much of the packaging work, it:

- runs the the doxygen -> docstring scripts
- creates a sdist from setup.py in tar.gz and zip formats
- builds the documentation files
- creates a 64-bit debian binary

The following files are output:

- :file:`gpstk/dev/dist/gpstk-2.x.tar.gz` [tar source archive to run setup.py on]
- :file:`gpstk/dev/dist/gpstk-2.x.zip` [zip source archive to run setup.py on]
- :file:`gpstk/dev/dist/gpstkpythondoc.zip` [Sphinx html documentation to go on PyPI]
- :file:`gpstk/dev/dist/deb_dist/python-gpstk_2.x-1_amd64.deb` [64-bit Debian binary]

.. note::
    The python package stdeb is used by the script to generate the debian. It can be found on PyPI: stdeb_.

.. _stdeb: https://pypi.python.org/pypi/stdeb



Uploading
*****************************************

1. Run the above packaging script
2. Upload the sdist to PyPI: ::

        python setup.py sdist upload

3. Login to your account on PyPI: https://pypi.python.org/pypi
4. Upload the documentation archive gpstk/dev/dist/gpstkpythondoc.zip
