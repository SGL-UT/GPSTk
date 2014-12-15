#!/bin/bash
#----------------------------------------
#
# Purpose:
#     Automate the use of CMake, SWIG, dOxygen, Sphinx, and distutils
#     to build and install the GPSTK C++ Library, C++ Applications, 
#     Python bindings, and documentation.
#
# Help:
#    $ ./build.sh -h
#
#----------------------------------------

#----------------------------------------
# Default values for paths. Options can override these.
#----------------------------------------

gpstk_root=$PWD
gpstk_install=$HOME/.local/gpstk             # default to a user install path
python_install=$(python -m site --user-site) # e.g., $HOME/.local/lib/python2.7/site-packages
python_root=$gpstk_root/swig
build_root=$gpstk_root/build

#----------------------------------------
# Number of cores for use by compiler
#     Default = all of them!
#----------------------------------------

case `uname` in
    Linux)
        last_core_index=`cat /proc/cpuinfo | grep "processor" | awk '{print $3}' | tail -1`
        num_cores=`echo "$last_core_index + 1" | bc`
        ;;
    Darwin)
        num_cores=`sysctl -n hw.ncpu`
        ;;
    *)
        num_cores=1
esac
num_threads=$num_cores

#----------------------------------------
# usage() function to use when help is needed:
#----------------------------------------

usage()
{
cat << EOF

usage:     $0 [-bcdehtuvz] [-j <num_threads>]

purpose:   This script automates and documents how to build and install GPSTk with CMake.

OPTIONS:

   -b     build_only       Do not do an install. This is useful if you want to build as yourself,
                           and then later use sudo to do a system install.

   -c     clean            Removes all files created from the build and install process.
                               $ rm -rf $gpstk_install/*;
                               $ rm -rf $python_install/gpstk*
                               $ git clean -fxd $build_root/
                               $ git clean -fxd $gpstk_root/swig/sphinx/

   -d     build_docs       Build, process, and install all documentation files.
                               * build Doxygen files (used for python docstrings)
                               * build Sphinx RST files into HTML documentation
                               * generate graphviz dependency graph (.DOT and .PDF files)

   -e     build_ext        builds /ext library in addition to the /core library

   -j     num_threads      For use with make, i.e. "make -j $num_threads"

   -h     help             Show this message

   -t     test_switch      turn on CMake/Ctest framework

   -u     user_install     GPSTk installs will be in the user home tree, not system paths
                               * with user_install
                                   C++ lib install path = $HOME/.local/gpstk/lib
                                   python install path  = $HOME/.local/lib/pythonX.X/site-packages/gpstk
                               * without user_install
                                   C++ lib install path = /usr/local/lib
                                   python install path  = /usr/local/lib/pythonX.X/dist-packages/gpstk

   -v     verbosity        Debug output

   -z     build_sdist      build source distribution for the python package and tar/zip it

EOF

exit 1
}

#----------------------------------------
# Parse input args
#----------------------------------------

while getopts "bcdehj:tuvz" option; do
    case $option in
        
        h) usage;;
        b) build_only=1;;
        c) clean=1;;
        d) build_docs=1;;
        e) build_ext=1;;
        j) num_threads=${OPTARG};;
        t) test_switch=1;;
        u) user_install=1;;
        v) verbosity=1;;
        z) build_sdist=1;;
        *) echo "Invalid option: -$OPTARG" >&2
           usage
           ;;
    esac
done

# Shift OPTIND so that if there are any additional 
# command line options we did not capture,
# then they are accessible via use of $*
shift $(($OPTIND - 1))

#----------------------------------------
# User Install
#----------------------------------------

if [ "$user_install" ]; then
    echo "$0: Install: User intall paths"
    gpstk_install=$HOME/.local/gpstk
    python_install=$(python -m site --user-site)
    build_root=$gpstk_root/build
else
    echo "$0: Install: System intall paths"
    build_root=/tmp/gpstk/build
    gpstk_install=/usr/local
    # python_install=$(python -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())")
	#    returns /usr/lib/ path vs. /usr/local/lib/ path
    python_install=$(python -c "import site; print(site.getsitepackages()[0])")
	#    returns /usr/local/lib/ path vs. /usr/lib/ path
fi

#----------------------------------------
# Echo configuration
#----------------------------------------

# helper function to print out configutaion
function ptof {
    if [ -z $1 ]; then printf "False";
    else               printf "True";
    fi
}

printf "$0: gpstk_root      = $gpstk_root\n"
printf "$0: gpstk_install   = $gpstk_install\n"
printf "$0: python_install  = $python_install\n"
printf "$0: user_install    = $(ptof $user_install)\n"
printf "$0: clean           = $(ptof $clean)\n"
printf "$0: build_root      = $build_root\n"
printf "$0: num_cores       = $num_cores\n"
printf "$0: num_threads     = $num_threads\n"
printf "$0: build_ext       = $(ptof $build_ext)\n"
printf "$0: test_switch     = $(ptof $test_switch)\n"
printf "$0: verbosity       = $(ptof $verbosity)\n"
printf "$0: build_docs      = $(ptof $build_docs)\n"
printf "$0: build_sdist     = $(ptof $build_sdist)\n"

if [ ! -d "$gpstk_root" ]; then
    echo "$0: COnfiguration: ERROR: $gpstk_root does not exist."
    exit 1
fi

#----------------------------------------
# Test for source files, exit if nothing to build
#----------------------------------------

num_files_ext=0
num_files_core=0
if [ "$build_ext" ]; then
    num_files_ext=`find $gpstk_root/ext/lib/ -type f -name "*.cpp" | wc -l`
    if [ "$num_files_ext" -eq 0 ]; then
        echo "$0: ERROR: $gpstk_root/ext/lib contains no *.cpp source files. Try building without /ext. See help: $0 -h"
        exit 1
    fi
else
    num_files_core=`find $gpstk_root/core/lib/ -type f -name "*.cpp" | wc -l`
    if [ "$num_files_core" -eq 0 ]; then
        echo "$0: ERROR: $gpstk_root/core/lib contains no *.cpp source files. Try building /ext. See help: $0 -h"
        exit 1
    fi
fi

#----------------------------------------
# Clean build and install
#----------------------------------------

if [ "$clean" ]; then
    echo ""
    echo ""
    echo "$0: Clean: Removing previous c++ build"
    echo ""
    echo ""

    echo "$0: Clean: Removing previous install directory"
    rm -rf $gpstk_install/*;
    rm -rf $python_install/gpstk*
    git clean -fxd $build_root/
    git clean -fxd $gpstk_root/swig/sphinx/
fi
mkdir -p $build_root
mkdir -p $gpstk_install

#----------------------------------------
# Pre-Build Documentation processing
#----------------------------------------

if [ "$build_docs" ]; then

    echo ""
    echo ""
    echo "$0: Documentation: processing dOxygen XML and docstrings and Sphinx RST ..."
    echo ""
    echo ""
	
    # build doxygen xml output from scraping the source files *.cpp and *.hpp
    if [ -d "$gpstk_root/doc" ]; then
        echo "$0: Documentation: Doxygen: Using existing doxygen files."
    else
        echo "$0: Documentation: Doxygen: Generating XML files and dot graphs for C++ library... $gpstk_rooot/doc/xml"
        cd $gpstk_root
        # Running doxygen walks the C++ source files and scrapes doxygen comments and 
        # writes out $gpstk_root/doc/html/*.html and $gpstk_root/doc/xml/*.xml
        # and generates graphviz files (.map, .md5, and .png) in $gpstk_root/doc/html/
        doxygen
        tar -czvf $gpstk_root/gpstk_doc_cpp.tgz -C $gpstk_root/doc/html .
        # doc_install_prefix=/usr/share/gpstk
        # mkdir -p $doc_install_prefix/cpp/html
        # tar -xzvf gpstk_doc_cpp.tgz -C $doc_install_prefix/cpp/html
    fi

    # generate python files.i docstrings from doxygen xml output
    if [ -d "$python_root/doc" ]; then
        echo "$0: Documentation: Docstrings: Using existing docstring files."
    else
        echo "$0: Documentation: Docstrings: using Doxygen XML output to generate docstring .i files for python bindings... $python_root/doc "
        cd $python_root
        # running docstring_generator.py reads in the $gpstk_root/doc/xml/*.xml 
        # fileswere created previously by doxygen and then 
        # outputs SWIG .i files to $python_root/doc
        python docstring_generator.py
    fi

    # make path for graphviz
    path_graphviz=$build_root/doc/graphviz
    mkdir -p $path_graphviz
	
fi

#----------------------------------------
# CMake generation of Makefiles
#----------------------------------------

echo ""
echo ""
echo "$0: CMake: executing cmake command to generate Makefiles..."
echo ""
echo ""

args=""
args+=${gpstk_install:+" -DCMAKE_INSTALL_PREFIX=$gpstk_install"}
args+=${user_install:+" -DPYTHON_USER_INSTALL=ON"}
args+=${build_ext:+" -DBUILD_EXT=ON"}
args+=${test_switch:+" -DTEST_SWITCH=ON"}
args+=${verbosity:+" -DDEBUG_SWITCH=ON"}
args+=${build_docs:+" --graphviz=$path_graphviz/gpstk_graphviz.dot"}

cd $build_root
cmake $args $gpstk_root

#----------------------------------------
# Build
#----------------------------------------

echo ""
echo ""
echo "$0: Make: Building C++ libs, C++ apps, and Python package..."
echo ""
echo ""

cd $build_root
# make install -j $num_cores
make -j $num_threads

#----------------------------------------
# Test: Hooks for test framework
#----------------------------------------

if [ "$test_switch" ]; then
    cd $build_root
    ctest -v
fi

#----------------------------------------
# Post-Build Documentation processing
#----------------------------------------

if [ "$build_docs" ]; then

    echo ""
    echo ""
    echo "$0: Post-build Documentation: generating HTML documentation for python bindings from RST files using Sphinx ..."
    echo ""
    echo ""

    # Building sphinx RST documentation
    # Requires the import of the gpstk python package, so you have to do this AFTER building the bindings
    # After importing gpstk, sphinx generates *.rst files for each attibute in the gpstk namespace, and
    # then converts those to html files.
    command -v sphinx-build 1>/dev/null 2>/dev/null # exit status 0 indicated program exists
    if [[ $? -eq 0 ]] ; then
        echo "$0: Documentation: Sphinx: Rebuilding RST documentation now..."
        cd $python_root/sphinx
        # running make html generates a lot of new RST files in $python_root/sphinx/*.rst
        # and a bunch of html files under $python_root/sphinx/_build/html/*.html
        make html
        tar -czvf $gpstk_root/gpstk_doc_python.tgz -C $python_root/sphinx/_build/html/ .
        # doc_install_prefix=/usr/share/gpstk
        # mkdir -p $doc_install_prefix/python/html
        # tar -xzvf gpstk_doc_python.tgz -C $doc_install_prefix/python/html


        # doc_install=/usr/share/gpstk/doc
        # mkdir -p $doc_install/python
        # tar -xzvf gpstk_doc_python.tgz -C $doc_install/python/

    else
        echo "$0: Documentation: Cannot build Sphinx Documentation."	
	fi
	

    echo ""
    echo ""
    echo "$0: Post-build Documentation: processing Graphviz ..."
    echo ""
    echo ""

    # Building Graphviz .DOT and .PDF
    command -v dot 1>/dev/null 2>/dev/null # exit status 0 indicated program exists
    if [[ $? -eq 0 ]] ; then
        echo "$0: Documentation: Graphviz: Generating GraphViz output PDF... $path_graphviz"
        dot -Tpdf $path_graphviz/gpstk_graphviz.dot -o $path_graphviz/gpstk_graphviz.pdf
    else
        echo "$0: Documentation: Cannot build Graphviz documentation."	
    fi

fi

#----------------------------------------
# Python source distribution
#----------------------------------------
# default: tar-ball, zip
# optional: debian, set package_debian=1 to build it.

if [ $build_sdist ]; then

    echo ""
    echo ""
    echo "$0: Distribution: Generating distribution packages..."
    echo ""
    echo ""
    cd $python_root/install_package
    python setup.py sdist --formats=zip,gztar

	package_debian=0
    if [ $package_debian = 1 ]; then
        # py2dsc will convert a distutils-built source tarball into a Debian source package.
        # py2dsc is not typically installed, so we need to add a check before trying to run it.
        command -v py2dsc 1>/dev/null 2>/dev/null # exit status 0 indicated program exists
        if [[ $? -eq 0 ]] ; then
            cd $python_root/install_package/dist
            py2dsc gpstk-2.5.tar.gz
            cd $python_root/install_package/dist/deb_dist/gpstk-2.5/
            dpkg-buildpackage -rfakeroot -uc -us
        fi
    fi
fi

#----------------------------------------
# Test LD_LIBRARY_PATH
#----------------------------------------

# Test whether $gpstk_install is in the user's PATH
# If not, echo a warning since the python extension module will break at run time.

echo ""
echo ""
echo "$0: Paths: Testing library load paths..."
echo ""
echo ""

if [[ ":$PATH:" == *":$gpstk_install/lib:"* ]]; then
    echo "$0: Paths: install location for libgpstk.so is in your PATH. Well done. No further action needed."
elif [[ ":$LD_LIBRARY_PATH:" == *":$gpstk_install/lib:"* ]]; then
    echo "$0: Paths: install location for libgpstk.so is in your LD_LIBRARY_PATH. Well done. No further action needed."
else
    echo "$0: Paths: install location for libgpstk.so is NOT in your path. You must add it to your environment to use it."
    echo "$0: Paths: recommend updating your LD_LIBVRARY_PATH as follows: "
    echo "$0:            $ export LD_LIBRARY_PATH=$gpstk_install/lib:\$LD_LIBRARY_PATH "
fi

#----------------------------------------
# Install
#----------------------------------------

if [ $build_only ]; then
    echo ""
    echo ""
    echo "$0: Install: Build script was configured to do a build only, no install."
    echo ""
    echo ""
else
    cd $build_root
    make install -j $num_threads
fi

#----------------------------------------
# The End
#----------------------------------------
echo ""
echo ""