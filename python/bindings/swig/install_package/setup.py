#!/usr/bin/env python
"""GPSTk python extension module installer.

Usage:
    global installation: $ python setup.py install 
    local installation:  $ python setup.py --install-lib=./my_packages
    other options:       [--user, --home=<install-path>, --prefix=<install-path>]

Examples:
    POSIX:   python setup.py install
    POSIX:   python setup.py --install-lib=~/.local/lib/python2.7/site-packages/
    Windows: python setup.py --install-lib=C:\Python27\Lib\site-packages

Purpose:
    Moves files into python module install path

Details:
    Default install path: ~/.local/lib/python<version>/site-packages/gpstk
    Files moved: _gpstk.so, gpstk.py, __init__.py
    Environment: export LD_LIBRARY_PATH=/path/of/libgpstk.so/root/dir/

    
"""
from distutils.core import setup

#----------------------------------------
# from distutils.core import Extension
#----------------------------------------
# setup( ...
#        ext_modules=[Extension( '_gpstk', ['gpstk.i'],
#                                library_dirs=['~/git/gpstk/install/lib'],
#                                libraries=['gpstk'],
#                                swig_opts=['-modern', '-I../include']
#                              )
#                    ],
#        ...
#      )
#----------------------------------------
# $ python setup.py build_ext
#----------------------------------------


#----------------------------------------
# File lists
#----------------------------------------

module_file_list = [ "gpstk.py", 
                     "_gpstk.so", 
                     "__init__.py"]

example_file_list = [ "examples/example1.py", 
                      "examples/example2.py", 
                      "examples/example3.py", 
                      "examples/example4.py", 
                      "examples/example5.py"]

test_file_list = [ "test/gpstk_store_test.py", 
                   "test/gpstk_time_test.py", 
                   "test/gpstk_util_test.py", 
                   "test/test.py"]

data_file_list = [ "data/rinexmet_data.txt",
                   "data/rinex3nav_data.txt", 
                   "data/rinex3obs_data.txt", 
                   "data/sem_data.txt", 
                   "data/sp3_data.txt", 
                   "data/yuma_data.txt"]

#----------------------------------------
# setup() input arg explanations:
#----------------------------------------
# packages = ['gpstk','test', 'examples'],
#             list of subdirectories which contain __init__.py files
#             e.g., 'gpstk' looks for "./gpstk/__init__.py"
# py_modules = ['gpstk'],
#             list of modules in the distribution root directory
#             e.g. 'my_module' looks for "./my_module.py"
#             You do NOT need to tell setup() about $root/gpstk/gpstk.py
#             if you already told it about $root/gpstk/__init__.py
# data_files=[ list of pairs (target_directory,[list of files]) ]
#
# runtime_library_dirs = [ list of directory_path strings ]
#    Each of the strings directory_path listed as the value of 
#    runtime_library_dirs identifies a directory where dynamically 
#    loaded libraries are found at runtime.
#

#----------------------------------------
# setup() call
#----------------------------------------

setup( name='gpstk',
       version='2.5',
       author='Space and Geophysics Lab, ARL, University of Texas at Austin',
       author_email = "gpstk@arlut.utexas.edu",
       url='http://www.gpstk.org/',
       description = "GPSTk Python package and extension module",
       long_descriptiopn = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, \
                            sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. \
                            Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris \
                            nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in \
                            reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla \
                            pariatur. Excepteur sint occaecat cupidatat non proident, sunt in \
                            culpa qui officia deserunt mollit anim id est laborum.",
       classifiers = ['License :: OSI Approved :: GNU Library or Lesser General Public License (LGPL)',
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
       keywords = ('GNSS', 'GPS', 'RINEX'),
       packages = ['gpstk'],
       scripts = [example_file_list + test_file_list],
       package_data = { 'gpstk' : data_file_list },
       data_files = [("data", data_file_list )]
       )

#----------------------------------------
# The End
#----------------------------------------
