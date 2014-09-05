#!/bin/bash
#----------------------------------------
#
# purpose = Automate the use of CMake and make to build C++ and Python GPSTk files and libraries
#
# outline = Optionally build C++, Python, or both files and libraries
#             By default, the Python GPSTk library is installed in the local Python site packages directory (~/.local/lib/pythonXX/site-packages on Linux)
#               If the option [-l] is specified, the user must specify a different install location
#               By default, the script purges the specified Python install folder prior to the install
#               in order to prevent file contamination with previous installations
#           Optionally clean out install and build paths
#           Export gpstk install path as environment variables used by CMake
#           Run CMake to generate build environment
#           Run make to build GPSTk
#           Run make install to install GPSTk
#           Echo a list of the installed files
#
#----------------------------------------

#----------------------------------------
# Default values for paths. Options can override these.
#----------------------------------------

gpstk_root=$PWD/dev
gpstk_install=$PWD/dev/install
python_install=$(python -m site --user-site)
python_root="$gpstk_root/../python/bindings/swig"

# Default to using all cores for compiles...
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

#----------------------------------------
# usage() function to use when help is needed:
#----------------------------------------

usage()
{
cat << EOF

usage:     $0 [-h] [-cpdbiotv] [-l <path>] [-r <path>] [-s <path>]

purpose:   This script is for use with CMake and building GPSTk.

OPTIONS:
 OPTIONS:
   -h     help             Show this message

   -c     build_cpp        build C++ files and libraries
   -d     build_doxygen    build Doxygen files (used for python docstrings)
   -p     build_python     build Python files and libraries with SWIG bindings

   -b     clean_build      rm -rf gpstk_root/build; rm -rf python_root/build
   -i     clean_install    rm -rf gpstk_install; rm -rf python_install
   
   -o     core_only        only builds core library code
   -t     test_switch      initialize test framework
   -v     graphviz         generate dependency graph (.DOT and .PDF files)

   -r     gpstk_root       default = $gpstk_root, path to GPSTk dev top-level CMakeLists.txt file (and source tree)
   -s     gpstk_install    default = $gpstk_install,  path to GPSTk install, to contain ./bin, ./lib, and ./include
                           If a relative path is supplied here, it needs to be relative to gpstk_root
   -l     python_install   default = $python_install, change Python library installation location

EOF

exit 1
}

#----------------------------------------
# Parse input args
#----------------------------------------

while getopts "hcdpbiotvl:r:s:" option; do
    case $option in
        
        h) usage;;
        c) build_cpp=1;;
        d) build_doxygen=1;;
        p) build_python=1;;
        b) clean_build=1;;
        i) clean_install=1;;
        o) core_only=1;;
        t) test_switch=1;;
        v) graphviz=1;;
        l) python_install=${OPTARG};;
        r) gpstk_root=${OPTARG};;
        s) gpstk_install=${OPTARG};;
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
# Echo configuration
#----------------------------------------

# helper function to print out configutaion
function ptof {
    if [ -z $1 ]; then printf "False";
    else               printf "True";
    fi
}

printf "$0: gpstk_root      = $gpstk_root\n"
printf "$0: build_cpp       = $(ptof $build_cpp)\n"
if [ "$build_cpp" ]; then
    printf "$0: core_only       = $(ptof $core_only)\n"
    printf "$0: test_switch     = $(ptof $test_switch)\n"
    printf "$0: graphviz        = $(ptof $graphviz)\n"
    printf "$0: gpstk_install   = $gpstk_install\n"
fi

printf "$0: clean_install   = $(ptof $clean_install)\n"
printf "$0: clean_build     = $(ptof $clean_build)\n"
printf "$0: build_doxygen   = $(ptof $build_doxygen)\n"
printf "$0: build_python    = $(ptof $build_python)\n"

if [ $build_python ]; then
    printf "$0: python_install  = $python_install\n"
fi

printf "$0: num_cores       = $num_cores\n"

if [ ! -d "$gpstk_root" ]; then
    echo "$0: Error. $gpstk_root does not exist."
    exit 1
fi

#----------------------------------------
# C++: Build and install lib and apps
#----------------------------------------
# Construct CMake command string based on script options
# Reminder: to set variables from command line use "$ cmake -D <var>:<type>=<value>"

if [ "$build_cpp" ]; then

    if [ "$clean_build" ]; then
        echo "$0: Removing previous c++ build"
        rm -rf "$gpstk_root/build"
    fi
    mkdir -p $gpstk_root/build

    # Generate build files (e.g. Makefiles)
    args=""
    args+=${gpstk_install:+" -DCMAKE_INSTALL_INSTALL=$gpstk_install"}
    args+=${core_only:+" -DCORE_ONLY=ON"}
    args+=${test_switch:+" -DTEST_SWITCH=ON"}
    args+=${graphviz:+" --graphviz=$path_graphviz/gpstk_graphviz.dot"}

    cd $gpstk_root/build
    cmake $args ..

    # Build library and apps
    echo "$0: Building c++ libs and apps"
    cd $gpstk_root/build
    make -j $num_cores

    # Install library and apps
    if [ "$clean_install" ]; then
        echo "$0: Removing previous install directory"
        rm -rf $gpstk_install
    fi
    mkdir -p $gpstk_install

    echo "$0: Installing c++ libs and apps"
    make install

    # Convert graphviz .DOT file into a .PDF
    if ["$graphviz" ]; then
        path_graphviz=$gpstk_install/graphviz
        mkdir -p $path_graphviz
        dot -Tpdf $path_graphviz/gpstk_graphviz.dot -o $path_graphviz/gpstk_graphviz.pdf
    fi

fi

#----------------------------------------
# Doxygen
#----------------------------------------
# Build Doxygen Files
if [ "$build_doxygen" ]; then
    cd $gpstk_root
    doxygen
    cd $python_root
    python docstring_generator.py
fi

#----------------------------------------
# Test: Hooks for test framework
#----------------------------------------

if [ "$test_switch" ]; then
    cd $gpstk_root/build
    ctest -v
fi

#----------------------------------------
# Python: Build/install swig bindings
#----------------------------------------

if [ "$build_python" ]; then
    if [ "$build_doxygen" ]; then
        if [ -d "$gpstk_root/doc" ]; then
            echo "Using existing doxygen files."
        else
            echo "Doxygen Files Not Found, Building now..."
            cd $gpstk_root
            doxygen
        fi

        if [ -d "$python_root/doc" ]; then
            echo "Using existing docstring files."
        else
            echo "Docstring Files Not Found, Building now..."
            cd $python_root
            python docstring_generator.py
        fi
    fi

    if [ "$clean_build" ]; then
        rm -rf  $python_root/build
    fi
    
    [ -d $python_root/build ] || mkdir -p $python_root/build

    cd $python_root/build
    cmake -DCMAKE_INSTALL_PREFIX=$python_install ..
    echo "$0: Building python gpstk module"
    make -j$num_cores

    if [ "$clean_install" ]; then
        rm -rf $python_install
    fi
    mkdir -p $python_install

    echo "$0: Installing python gpstk module"
    make install
fi

#----------------------------------------
# Test paths
#----------------------------------------

# Test whether $gpstk_install is in the user's PATH
# If not, echo a warning since the python extension module will break at run time.

if [[ ":$PATH:" == *":$gpstk_install/lib:"* ]]; then
    echo "NOTICE: Your chosen install location for libgpstk.so is in your PATH. Well done."
elif [[ ":$LD_LIBRARY_PATH:" == *":$gpstk_install/lib:"* ]]; then
    echo "NOTICE: Your chosen install location for libgpstk.so is in your LD_LIBRARY_PATH. Well done."
else
    echo "WARNING: Your chosen install location for libgpstk.so is NOT in your path."
    echo "         You may want to add it to your environment LD_LIBRARY_PATH like so:"
    echo "             $ export LD_LIBRARY_PATH=$gpstk_install/lib:'$LD_LIBRARY_PATH' "
fi

#----------------------------------------
# The End
#----------------------------------------
