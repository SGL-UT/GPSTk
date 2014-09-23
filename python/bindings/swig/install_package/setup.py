#!/usr/bin/env python
"""GPSTk python package installer.

Purpose:
    Install GPSTk python package using setuptools or distutils

Usage:
    help:           $ python setup.py --help-commands
    system install: $ python setup.py install 
    user install:   $ python setup.py install --prefix=~/.local
    documentation:  https://docs.python.org/2/install/

Examples:
    POSIX:   python setup.py install --prefix=~/.local
    Windows: python setup.py install --install-lib=C:\Python27\Lib\site-packages

Details:
    Default install path: ~/.local/lib/python<version>/site-packages/gpstk
    Environment: $ export LD_LIBRARY_PATH=/path/to/intall/of/libgpstk.so

"""

from distutils.core import setup, Extension

#----------------------------------------
# File lists
#----------------------------------------

# starting location of example files
example_file_list = [ "examples/example1.py",
                      "examples/example2.py",
                      "examples/example3.py",
                      "examples/example4.py",
                      "examples/example5.py"]

# For package distribution channels
classifiers_list = ['License :: OSI Approved :: GNU Library or Lesser General Public License (LGPL)',
                    'Programming Language :: Python :: 2.7',
                    'Programming Language :: C++',
                    'Operating System :: POSIX :: Linux',
                    'Environment :: Console',
                    'Environment :: Other Environment',
                    'Framework :: IPython',
                    'Natural Language :: English',
                    'Development Status :: 4 - Beta',
                    'Intended Audience :: Developers',
                    'Intended Audience :: Science/Research',
                    'Topic :: Software Development :: Libraries :: Python Modules',
                    'Topic :: Scientific/Engineering :: Physics',
                    'Topic :: Scientific/Engineering :: Atmospheric Science',
                    'Topic :: Scientific/Engineering :: GIS']

# Dependencies for easy_install (may not be needed)
requirements_list = [ "python >= 2.7.3",
                      "numpy >= 1.4.4",
                      "doxygen >= 1.8.1",
                      "cmake >= 2.8",
                      "make >= 3.81",
                      "gcc >= 4.7.2",
                      "swig >= 2.0",
                      "setuptools >= 0.6",
                      ]

#----------------------------------------
# setup() call
#----------------------------------------

setup( name              = "gpstk",
       version           = "2.5",
       author            = "Space and Geophysics Lab, ARL, University of Texas at Austin",
       author_email      = "gpstk@arlut.utexas.edu",
       url               = "http://www.gpstk.org/",
       license           = "LICENSE.txt",
       description       = "GPSTk python package installer",
       long_description  = open('README.txt').read(),
       keywords          = ('GNSS', 'GPS', 'RINEX'),
       classifiers       = classifiers_list,
       packages          = ['gpstk', 'gpstk.test', 'gpstk.data'],
       package_data      = { 'gpstk': ['data/*.txt', '_gpstk.so'],},
       scripts           = example_file_list,
       )

#----------------------------------------
# The End
#----------------------------------------
