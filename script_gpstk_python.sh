#!/bin/bash
#----------------------------------------
#
# purpose = Automate the use of CMake and make to build the Python GPSTk
#
# outline = Optionally clean out install and build paths
#           Export gpstk install path as environment variables used by CMake
#           Run CMake to generate build environment
#           Run make to build the Python GPSTk
#           Run make install to install the Python GPSTk
#           Echo a list of the installed files
#           By default, the Python GPSTk library is installed in the local Python site packages directory (~/.local/lib/pythonXX/site-packages on Linux)
#             If the option [-p] is specified, the user must specify a different install location
#           By default, the script purges the specified install folder prior to the install in order to prevent file contamination with previous installations
#
# usage:    ./script_gpstk_python.sh [-h] [-d] [-b] [-r <path>] [-l <path>]
# OPTIONS:
#    -h     help,          Show this message
#    -d     build_doxygen, build doxygen files
#    -b     clean_build,   rm -rf ~/git/gpstk/python/bindings/swig/build
#    -r     gpstk_root,    default = ~/git/gpstk/dev, path to GPSTk top-level CMakeLists.txt file (and source files tree)
#    -l     python_install, default = ~/.local/lib/pythonX.X/site-packages, path to python library install directory to contain Python GPSTk library files
#
#----------------------------------------

#----------------------------------------
# usage() function to use when help is needed:
#----------------------------------------

usage()
{
cat << EOF

usage:     $0 [-h] [-d] [-b] [-r <path>] [-l <path>]

purpose:   This script is for use with CMake and building GPSTk.

OPTIONS:
   -h      help,          Show this message
   -d      build_doxygen, build doxygen files
   -b      clean_build,   rm -rf ~/git/gpstk/python/bindings/swig/build
   -r      gpstk_root,    default = ~/git/gpstk/dev, GPSTK top-level CMakeLists.txt file (and source files tree)
   -l      python_install, default = ~/.local/lib/pythonX.X/site-packages, Python install directory 
                                    (If no Python directory specified, /python directory will be created at specified location)
  
EOF

exit 1
}

#----------------------------------------
# initialize variables as empty
#----------------------------------------

build_doxygen=
clean_build=
gpstk_root=
python_install=
change_python_loc=

#----------------------------------------
# initialize Color Codes
#----------------------------------------

green='\e[1;32m'
red='\e[1;31m'
normal='\e[0m'
white='\e[1;37m'
cyan='\e[1;36m'
purple='\e[1;35m'
brown='\e[1;33m'

#----------------------------------------
# Parse input arguments
#----------------------------------------

while getopts ":r:l:hbd" option; do

  case $option in

    h)
      usage
      ;;

    d)
      build_doxygen=1
      ;;

    b)
      clean_build=1
      ;;

    r)
      gpstk_root=${OPTARG}
      ;;

    l)
      python_install=${OPTARG}
      ;;
 
    \?)
      echo "Invalid option: -$OPTARG" >&2
      usage
      ;;

  esac
done

# Shift OPTIND so that if there are any additional 
# command line options we did not capture,
# then they are accessible via use of $*
shift $(($OPTIND - 1))

#----------------------------------------
# Set input arg defaults, if empty
#----------------------------------------

if [ -z $build_doxygen ]
    then
        build_doxygen=0
        echo "$0: Input arg: build_doxygen = $build_doxygen (default)"
    else
        echo "$0: Input arg: build_doxygen = $build_doxygen (user provided)"
fi

if [ -z $clean_build ]
    then
        clean_build=0
        echo "$0: Input arg: clean_build   = $clean_build (default)"
    else
        echo "$0: Input arg: clean_build   = $clean_build (user provided)"
fi

if [ -z $gpstk_root ]
    then
        gpstk_root=~/git/gpstk/dev
        echo "$0: Input arg: gpstk_root    = $gpstk_root (default)"
    else
        echo "$0: Input arg: gpstk_root    = $gpstk_root (user provided)"
fi

if [ -z $python_install ]
    then
        python_install=$(python -m site --user-site) # Determine local Python site-packages location
        echo "$0: Input arg: python_install = $python_install (default)"
        change_python_loc=0
    else
        python_install=$python_install"/python"
        echo "$0: Input arg: python_install = $python_install (user provided)"
        change_python_loc=1
fi

#----------------------------------------
# Test gpstk paths before doing anything
#----------------------------------------

if [ -d $gpstk_root ]; 
    then
        echo "$0: Paths: $gpstk_root, directory exist test = PASS"
    else
        echo "$0: Paths: $gpstk_root, directory exist test = FAIL"
        echo "$0: exitting..."
        exit 1
fi

if [ ! -d $python_install ]; 
    then
        echo "$0: Paths: Creating $python_install directory"
        mkdir -p $python_install
    else 
        echo "$0: Paths: $python_install, directory exist test = PASS"
fi

#----------------------------------------
# Optionally clean out the last GPSTk build
#----------------------------------------

if [[ "$clean_build" == 0 ]]
    then
        echo "$0: Options: Previous build to be removed = FALSE"
    else
        echo "$0: Options: Previous build to be removed = TRUE"
        rm -rf $gpstk_root/../python/bindings/swig/build
        mkdir $gpstk_root/../python/bindings/swig/build
fi

#----------------------------------------
# Clean out the last Python GPSTk install
#----------------------------------------

if [[ "$change_python_loc" == 0 ]]
    then
        rm -rf $python_install"/gpstk"
        rm -rf $gpstk_root"/../install/python"
    else 
        rm -rf $gpstk_root"/../install/python"
fi

#----------------------------------------
# Test and then export install paths needed by CMake
#----------------------------------------

gpstk_dir=$gpstk_root"/../install"

if [ -d $gpstk_dir ]
    then
        echo "$0: Paths: $python_install, directory exist test = PASS"
    else
        echo "$0: Paths: $python_install, directory exist test = FAIL"
        echo "$0: exitting..."
        exit 1
fi

# This is required by the current CMake set-up
echo "$0: Paths: Exporting install paths..."

export gpstk=$gpstk_dir

#----------------------------------------
# Doxygen
#----------------------------------------

# Build Doxygen Files
if [[ "$build_doxygen" == 0 ]]
    then
        echo "$0: Options: generate doxygen files = FALSE"
    else
        echo "$0: Options: generate doxygen files = TRUE"
        
        cd $gpstk_root
        doxygen
        cd ../python/bindings/swig
        python docstring_generator.py
fi

#----------------------------------------
# Python Bindings
#----------------------------------------

# Build Python Swig Bindings
# Checks to see if Doxygen built /dev/doc folder exists, if not it builds it

if [ -d $gpstk_root"/doc" ]
    then
        echo "Doxygen Files Found"
    else
        echo "Doxygen Files Not Found, Building now..."
        cd $gpstk_root
        doxygen
fi

echo "$0: Options: build python bindings = TRUE"

if [ -d $gpstk_root/../python/bindings/swig/doc ]
    then 
        echo "Docstring Files Found"
    else
        echo "Docstring Files Not Found, Building now..."
        cd ../python/bindings/swig
        python docstring_generator.py 
fi

cd $gpstk_root/../python/bindings/swig
[ -d build ] || mkdir build
cd build
cmake ..
make -j$num_cores

cd ../bin
python gpstk_builder.py $python_install

if [[ "$change_python_loc" == 1 ]]
    then 
        cp -r $python_install"/gpstk/"* $python_install
        rm -rf $python_install"/gpstk"
        python_loc_d=$(python -m site --user-site)"/gpstk"
        rm -r $python_loc_d
        ln -s $python_install $python_loc_d
fi

#----------------------------------------
# Echo the contents of the install paths
#----------------------------------------

cd $python_install
echo ""

if [[ "$change_python_loc" == 0 ]]
    then 
        echo ""
        echo "$0: Installed python libraries: $python_install/gpstk/"
        ls $python_install/gpstk
    else 
        echo ""
        echo "$0: Installed python libraries: $python_install/"
        ls $python_install
fi

echo ""
echo ""
echo "$0: ...done."
echo ""
echo ""

#----------------------------------------
# The End
#----------------------------------------