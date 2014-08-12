#!/bin/bash
#----------------------------------------
#
# purpose = Automate the use of CMake and make to build C++ and Python GPSTk files and libraries
#
# outline = Optionally build C++, Python, or both files and libraries
#             By default, the Python GPSTk library is installed in the local Python site packages directory (~/.local/lib/pythonXX/site-packages on Linux)
#               If the option [-l] is specified, the user must specify a different install location
#               By default, the script purges the specified Python install folder prior to the install in order to prevent file contamination with previous installations
#           Optionally clean out install and build paths
#           Export gpstk install path as environment variables used by CMake
#           Run CMake to generate build environment
#           Run make to build GPSTk
#           Run make install to install GPSTk
#           Echo a list of the installed files
#
#----------------------------------------

# Default values for paths. Options can override these.
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

usage:     $0 [-h] [-c] [-d] [-p] [-b] [-i] [-o] [-t] [-v] [-l <path>] [-r <path>] [-s <path>]

purpose:   This script is for use with CMake and building GPSTk.

OPTIONS:
 OPTIONS:
   -h     help             Show this message

   -c     build_cpp        build C++ files and libraries
   -d     build_doxygen    build Doxygen files
   -p     build_python     build Python files and libraries with SWIG bindings (autobuilds Doxygen)

   -b     clean_build      rm -rf gpstk_root/build
   -i     clean_install    rm -rf gpstk_install
   
   -o     core_only        only builds core library code
   -t     test_switch      initialize test framework
   -v     graphviz         generate dependency graph (.DOT and .PDF files)

   -l     python_install   default = $python_install, change Python library installation location
   -r     gpstk_root       default = $gpstk_root, path to GPSTk dev top-level CMakeLists.txt file (and source tree)
   -s     gpstk_install    default = $gpstk_install,  path to GPSTk install, to contain ./bin, ./lib, and ./include
                           If a relative path is supplied here, it needs to be relative to gpstk_root
EOF

exit 1
}

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

# helper function to print out configutaion
function ptof {
    if [ -z $1 ]; then printf "False";
    else               printf "True";
    fi
}

printf "$0: gpstk_root      = $gpstk_root\n"
printf "$0: build_cpp       = $(ptof $build_cpp)\n"
if [ "$build_cpp" ]; then
    printf "$0: clean_build     = $(ptof $clean_build)\n"
    printf "$0: clean_install   = $(ptof $clean_install)\n"
    printf "$0: core_only       = $(ptof $core_only)\n"
    printf "$0: test_switch     = $(ptof $test_switch)\n"
    printf "$0: graphviz        = $(ptof $graphviz)\n"
    printf "$0: gpstk_install   = $gpstk_install\n"
fi

printf "$0: build_doxygen   = $(ptof $build_doxygen)\n"
printf "$0: build_python    = $(ptof $build_python)\n"

if [ $build_python ]; then
    printf "$0: python_install  = $python_install\n"
fi

printf "$0: num_cores       = $num_cores\n"


#----------------------------------------
# Test build and install paths before doing anything
#----------------------------------------
if [ ! -d "$gpstk_root" ]; then
    echo "$0: $gpstk_root, directory exist test = FAIL"
    echo "$0: exitting..."
    exit 1
fi

if [ "$build_cpp" ]; then
    if [ ! -d "$gpstk_install" ]; then
        echo "$0: Creating $gpstk_install directory"
        mkdir -p $gpstk_install
    fi
    if [ ! -d "$gpstk_root/build" ]; then
        echo "$0: Creating $gpstk_root/build directory"
        mkdir -p $gpstk_root/build
    fi
fi

if [[ "$build_python" && ! -d "$python_install" ]]; then
    echo "$0: Creating $python_install directory"
    mkdir -p "$python_install"
fi

path_graphviz=$gpstk_install/graphviz
if [[ "$graphviz" && ! -d "$path_graphviz" ]]; then
    echo "$0: Creating $path_graphviz directory"
    mkdir -p $path_graphviz
fi

#----------------------------------------
# Optionally clean out previous builds and installs
#----------------------------------------
if [[ "$build_cpp" && "$clean_build" ]]; then
   echo "$0: Removing previous c++ build"
   rm -rf "$gpstk_root/build"
   mkdir -p "$gpstk_root/build"
fi

if [[ "$build_python" && "$clean_build" ]]; then
    echo "$0: Removing previous python build"
    rm -rf $gpstk_root/../python/bindings/swig/build
    mkdir -p $gpstk_root/../python/bindings/swig/build
    rm -rf $python_install"/gpstk"
fi

if [[ "$build_cpp" && "$clean_install" ]]; then
    echo "$0: Removing previous install directory"
    rm -rf $gpstk_install
    mkdir -p $gpstk_install
fi

#----------------------------------------
# Test and then export install paths needed by CMake
#----------------------------------------

if [[ ! -d "$gpstk_install" ]]; then
    echo "$0: Paths: $gpstk_install, directory exist test = FAIL"
    echo "$0: exitting..."
    exit 1
fi

export gpstk=$gpstk_install


#----------------------------------------
# CMake
#----------------------------------------
# Construct CMake command string based on script options
# Reminder: to set variables from command line use "$ cmake -D <var>:<type>=<value>"
if [ "$build_cpp" ]; then
    args=""
    args+=${core_only:+" -DCORE_ONLY=ON"}
    args+=${test_switch:+" -DTEST_SWITCH=ON"}
    args+=${graphviz:+" --graphviz=$path_graphviz/gpstk_graphviz.dot"}

    cd $gpstk_root/build
    cmake $args ..

    # Convert graphviz .DOT file into a .PDF
    if ["$graphviz" ]; then
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
# Build/install python swig bindings
#----------------------------------------
if [ "$build_python" ]; then
    if [ -d "$gpstk_root/doc" ]
    then
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

    cd $python_root/build
    cmake ..
    make -j$num_cores

    cd $python_root/bin
    python gpstk_builder.py $python_install
fi


#----------------------------------------
# Build/install c++ library and apps
#----------------------------------------
if [[ "$build_cpp" ]]; then
    echo "$0: Calling make and make install..."
    cd $gpstk_root/build
    make -j $num_cores
    make install
fi

if [ "$test_switch" ]; then
    cd $gpstk_root/build
    ctest -v
fi

echo ""
echo "$0: ...done."

