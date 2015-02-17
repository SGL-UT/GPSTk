#!/bin/bash
#----------------------------------------
#
# Purpose: GPSTk build and install script
#
#     Automate the use of CMake, SWIG, dOxygen, Sphinx, and distutils
#     to build and install the GPSTK C++ Library, C++ Applications, 
#     Python bindings, and documentation.
#
# Help:
#    $ ./build.sh -h
#
#----------------------------------------

#----------------------------------------
# Qué hora es? Dónde estamos? Y dónde vamos?
#----------------------------------------
 
build_date=`date +"%Y%m%d"`
build_time=`date +"%Y%m%d_%H%M%S"`
build_host=`hostname`
build_who=`whoami`

gpstk_root=$PWD
python_root=$gpstk_root/swig
python_exe=`which python2.7`

system_install_prefix=/usr/local
system_gpstk_install=/usr/local/gpstk
system_python_install=$(${python_exe} -c "import site; print(site.getsitepackages()[0])") # this will break for python < 2.7
system_build_root=/tmp/gpstk_build_${build_who}_${build_host}_${build_date}

user_install_prefix=$HOME/.local
user_gpstk_install=$HOME/.local/gpstk
user_python_install=$(${python_exe} -m site --user-site)
user_build_root=$gpstk_root/build

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

purpose:   This script automates and documents how to build, test, and install GPSTk with CMake.

usage:     $0 [-bcdehptuvz] [-j <num_threads>] [-i <install_prefix>] [-P <python_exe>]

           example:  Typical admin build and install: $ ./build.sh -e
           example:  Typical  user build and install: $ ./build.sh -eu
           example:  Typical  test build and install: $ ./build.sh -et -i /tmp/test

OPTIONS:

   -h     help             Show this message

   -b     build_only       Do not do an install. This is useful if you want to build as yourself,
                           and then later use sudo to do a system install.
                           Build path is constructed in this script.
                           Currently set to $system_build_root

   -e     build_ext        GPSTk has two parts: core and ext. See README.txt for details.
                           Default (without -e) will build only $gpstk_root/core
                           Optional (with -e) will build both $gpstk_root/core and $gpstk_root/ext

   -j     num_threads      Default = $num_threads, number of system cores determined from /proc/cpuinfo
                               for use with make, i.e. "make -j $num_threads"

   -t     test_switch      Execute all CMake Ctest test cases before install.

   -i     install_prefix   Default = $system_install_prefix, such that:
                               *  C++    install path = $system_gpstk_install
                               *  Python install path = $system_python_install
                           If you use a non-standard install path, you must update your environment.
                           For example, if your install_prefix=/tmp/test, do the following:
                               $ export LD_LIBRARY_PATH=/tmp/test/gpstk/lib:\$LD_LIBRARY_PATH
                               $ export PYTHONPATH=/tmp/test/lib/python2.7/site-packages:\$PYTHONPATH

   -u     user_install     Over-rides install prefix such that:
                               *  C++    install path = $user_gpstk_install
                               *  Python install path = $user_python_install
                           Also sets the build root to the following:
                               $user_build_root

   -d     build_docs       Build, process, and install all documentation files.
                               * build Doxygen files (used for python docstrings)
                               * build Sphinx RST files into HTML documentation
                               * generate graphviz dependency graph (.DOT and .PDF files)

   -z     build_sdist      build source distribution for the python package and tar/zip it

   -c     clean            Be very careful! Removes all files created from the build and install
                           Behaviour changes with install_prefix. Default with system install:
                               $ git clean -fxd $gpstk_root/swig/sphinx/
                               $ rm -rf $build_root
                               $ rm -rf $system_gpstk_install
                               $ rm -rf $system_python_install/gpstk*

   -p     build_python     Flag to build python extension package and set path to python intepreter
                           Default bit is build_python=0, but "-p" will set it to build_python=1

   -P     python_exe       Python executable used to help determine with python system libraries
                           will be used when building python extension package.
                           Default=`which python`, full system path to python executable
                           If this flag is used, automatically turns on -p also, i.e. sets build_python=1

   -v     verbosity        Debug output

EOF

exit 1
}

#----------------------------------------
# Parse input args
#----------------------------------------

while getopts "bcdehi:j:P:ptuvz" OPTION
do
    case $OPTION in
    b)   build_only=1;;
    c)   clean=1;;
    d)   build_docs=1;;
    e)   build_ext=1;;
    h)   usage;;
    i)   install_prefix="$OPTARG";;
    j)   num_threads="$OPTARG";;
    p)   build_python=1;;
    P)   build_python=1
         python_exe="$OPTARG"
         ;;
    t)   test_switch=1;;
    u)   user_install=1;;
    v)   verbosity=1;;
    z)   build_sdist=1;;
    *)   echo "Invalid option: -$OPTARG" >&2
         usage
         exit 2
         ;;
    esac
done
# Shift OPTIND so that if there are any additional 
# command line options we did not capture,
# then they are accessible via use of $*
shift $(($OPTIND - 1))

#----------------------------------------
# Install Paths
#----------------------------------------

if [[ -z "$install_prefix" ]]; then

    if [ "$user_install" ]; then
        install_prefix=$user_install_prefix
        gpstk_install=$user_gpstk_install
        build_root=$user_build_root
    else
        install_prefix=$system_install_prefix
        gpstk_install=$system_gpstk_install
        build_root=$system_build_root
    fi

else

    gpstk_install=${install_prefix}
    build_root=$system_build_root

fi

# the python install is ALWAYS in the install_prefix
# to insure, in the case of the default(system) install and the use install,
# that the pyhton package is installed to a place that is already in sys.path
# and to further insure that if the user selects a crazy non-standard install path,
# that the python stuff gets shoved into the same crazy place as the gpstk cpp junk
if [ "$build_python" ]; then
    python_install=${install_prefix}
else
    python_install=
fi

#----------------------------------------
# Echo configuration
#----------------------------------------

echo ""
echo ""
echo "============================================================"
echo "GPSTk Build and Test Script: Echo of config values..."
echo "============================================================"
echo ""
echo "$0: system user     = $build_who"
echo "$0: system name     = $build_host"
echo "$0: system time     = $build_time"
echo ""

# helper function: Print True Or False
function ptof {
    if [ -z $1 ]; then printf "False";
    else               printf "True";
    fi
}

printf "$0: gpstk_root      = $gpstk_root\n"
printf "$0: install_prefix  = $install_prefix\n"
printf "$0: gpstk_install   = $gpstk_install\n"
printf "$0: build_python    = $(ptof $build_python)\n"
printf "$0: python_exe      = $python_exe\n"
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
    echo "$0: Configuration: ERROR: $gpstk_root does not exist."
    exit 1
fi

echo ""
echo "============================================================"
echo ""

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
# Clean build and install directories
#----------------------------------------

if [ "$clean" ]; then
    echo ""
    echo ""
    echo "$0: Clean: Removing previous c++ and python build"
    echo ""
    echo ""
    
    git clean -fxd $gpstk_root/swig/sphinx/
    rm -rf $build_root

    if [ ! $build_only ]; then
        rm -rf $gpstk_install/*;
        rm -rf $python_install/gpstk*
    fi
fi

#----------------------------------------
# Try to create the build and install paths
#----------------------------------------

if [ ! -d "$build_root" ]; then
    echo "$0: NOTICE: build path $build_root does not exist. Creating it now."
    mkdir -p $build_root
    if [ ! -d "$build_root" ]; then
        echo "$0: ERROR: Unable to create the GPSTk build directory $build_root"
        exit 1
    fi
fi

if [ ! -d "$gpstk_install" ]; then
    echo "$0: WARNING: install path $gpstk_install does not exist. Creating it now."
    mkdir -p $gpstk_install
    if [ ! -d "$gpstk_install" ]; then
        echo "$0: ERROR: Unable to create the GPSTk C++ lib install directory $gpstk_install"
        exit 1
    fi
fi

if [ "$build_python" ]; then
    if [ ! -d "$python_install" ]; then
        echo "$0: WARNING: Install path $python_install does not exist. Creating it now."
        mkdir -p $python_install
        if [ ! -d "$python_install" ]; then
            echo "$0: ERROR: Unable to create the GPSTk Python package install directory $python_install"
            exit 1
        fi
    fi
fi

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
    if [ "$build_python" ]; then
        if [ -d "$python_root/doc" ]; then
            echo "$0: Documentation: Docstrings: Using existing docstring files."
        else
            echo "$0: Documentation: Docstrings: using Doxygen XML output to generate docstring .i files for python bindings... $python_root/doc "
            cd $python_root
            # running docstring_generator.py reads in the $gpstk_root/doc/xml/*.xml 
            # fileswere created previously by doxygen and then 
            # outputs SWIG .i files to $python_root/doc
            ${python_exe} docstring_generator.py
        fi
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
args+=${build_python:+" -DBUILD_PYTHON=ON"}
if [ "$build_python" ]; then
    args+=${python_exe:+" -DPYTHON_EXECUTABLE=$python_exe"}
    args+=${install_prefix:+" -DPYTHON_INSTALL_PREFIX=$python_install"}
fi
args+=${build_ext:+" -DBUILD_EXT=ON"}
args+=${test_switch:+" -DTEST_SWITCH=ON"}
args+=${verbosity:+" -DDEBUG_SWITCH=ON"}
args+=${build_docs:+" --graphviz=$path_graphviz/gpstk_graphviz.dot"}

#   args+="-C $gpstk_root/BuildSetup.cmake"

cd $build_root
CMAKE_OUTPUT_LOG=$build_root/cmake.log
cmake $args $gpstk_root 2>&1 | tee -a $CMAKE_OUTPUT_LOG

#----------------------------------------
# Build
#----------------------------------------

echo ""
echo ""
if [ "$build_python" ]; then
    echo "$0: Make: Building the following: C++ libs, C++ apps, and Python extension package"
else
    echo "$0: Make: Building the following: C++ libs, C++ apps"
fi
echo ""
echo ""

cd $build_root
MAKE_OUTPUT_LOG=$build_root/build.log
make -j $num_threads 2>&1 | tee -a $MAKE_OUTPUT_LOG

#----------------------------------------
# Test: Hooks for test framework
#----------------------------------------

if [ "$test_switch" ]; then

    # run the tests
    echo ""
    echo "------------------------------------------------------------"
    echo "Tests: started"
    echo "------------------------------------------------------------"
    echo ""

    cd $build_root
    ctest -v

    echo ""
    echo "------------------------------------------------------------"
    echo "Tests: completed"
    echo "------------------------------------------------------------"
    echo ""

    #----------------------------------------
    # capture the test output
    #----------------------------------------

    ctest_keyword=GpstkTest
    ctest_log_raw=$build_root/Testing/Temporary/LastTest.log
    ctest_log_save=$build_root/test.log
    cat $ctest_log_raw | grep "$ctest_keyword" > $ctest_log_save

    #----------------------------------------
    # summary results
    #----------------------------------------

    test_count=`cat $ctest_log_save | wc -l`
    tests_passed=`cat $ctest_log_save | grep "failBit=0" | wc -l`
    tests_failed=`cat $ctest_log_save | grep "failBit=1" | wc -l`

    echo ""
    echo "------------------------------------------------------------"
    echo "Tests: Summary Results"
    echo "------------------------------------------------------------"
    echo "Number of tests run    = $test_count"
    echo "Number of tests passed = $tests_passed"
    echo "Number of tests failed = $tests_failed"
    echo "------------------------------------------------------------"
    echo ""

    #----------------------------------------
    # Search test.log for keywords, summarize
    #----------------------------------------

    keyword_list=()
    keyword_list+=("TimeSystem")
    keyword_list+=("TimeConverters")
    keyword_list+=("TimeString")
    keyword_list+=("TimeTag")
    keyword_list+=("CommonTime")
    keyword_list+=("CivilTime")
    keyword_list+=("ANSITime")
    keyword_list+=("UnixTime")
    keyword_list+=("YDSTime")
    keyword_list+=("GPSWeek")
    keyword_list+=("MJD")
    keyword_list+=("JulianDate")
    keyword_list+=("Rinex")
    keyword_list+=("RinexObs")
    keyword_list+=("RinexNav")
    keyword_list+=("RinexMet")
    keyword_list+=("Binex")
    keyword_list+=("SP3")
    keyword_list+=("Ephemeris")
    keyword_list+=("Store")
    keyword_list+=("Position")
    keyword_list+=("Xvt")
    keyword_list+=("ObsID")
    keyword_list+=("SatID")
    keyword_list+=("Matrix")
    keyword_list+=("Vector")
    keyword_list+=("Triple")
    keyword_list+=("Stats")
    keyword_list+=("RACRotation")
    keyword_list+=("StringUtils")


    myheader="%-16s, %4s, %4s, %4s, %5s\n"
    myformat="%-16s, %4d, %4d, %4d, %5.1f \n"
    mydivider="========================================="


    # myformat="name = %-16s, tests run = %4d, passed = %4d, failed = %4d, %%passed = %5.1f \n"

    echo "------------------------------------------------------------"
    echo "Tests: Results by Category"
    echo "------------------------------------------------------------"

    echo ""
    printf "$myheader" "NAME" "RUN" "PASS" "FAIL" "%PASS"
    printf "%s\n" "$mydivider"

    for keyword in "${keyword_list[@]}"; do

        tests_run=`cat $ctest_log_save | grep -i "$keyword" | wc -l`
        tests_passed=`cat $ctest_log_save | grep -i "$keyword" | grep "failBit=0" | wc -l`
        tests_failed=`cat $ctest_log_save | grep -i "$keyword" | grep "failBit=1" | wc -l`
        percent_pass=$(awk -v r=$tests_run -v p=$tests_passed 'BEGIN { print (100*(p/r)) }')

        printf "$myformat" "$keyword" "$tests_run" "$tests_passed" "$tests_failed" "$percent_pass"

    done
    echo ""
    echo "------------------------------------------------------------"
    echo ""

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
    if [ "$build_python" ]; then
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

if [ "$build_python" ]; then
    if [ $build_sdist ]; then

        echo ""
        echo ""
        echo "$0: Distribution: Generating distribution packages..."
        echo ""
        echo ""
        cd $python_root/install_package
        ${python_exe} setup.py sdist --formats=zip,gztar

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
fi

#----------------------------------------
# Install
#----------------------------------------

if [ $build_only ]; then
    echo ""
    echo ""
    echo "$0: Install: Build script was configured to do a build only"
    echo "$0: Install: Exiting now..."
    echo ""
    echo ""
    exit 1
else
    cd $build_root
    INSTALL_OUTPUT_LOG=$build_root/install.log
    make install -j $num_threads 2>&1 | tee -a $INSTALL_OUTPUT_LOG
fi

#----------------------------------------
# Test Shell Environment, PATH, LD_LIBRARY_PATH
#----------------------------------------

# Test whether $gpstk_install is in the user's PATH
# If not, echo a warning since the python extension
# module will break at run time.

echo ""
echo ""
echo "$0: Paths: Testing library load paths..."
echo ""
echo ""

ldd_path_list=`ldconfig -v 2>/dev/null | grep -v ^$'\t' | sed -e 's/://g'`
ld_lib_path_list=`echo $LD_LIBRARY_PATH`
gpstk_install_path_test=`echo "$ldd_path_list $ld_lib_path_list" | grep -o "$gpstk_install"`

if [ -z "$gpstk_install_path_test" ]; then

    echo ""
    echo "$0: Paths: |------------------------------------------ "
    echo "$0: Paths: | [FAIL]: Post-install config test: Can $gpstk_install/lib be found by ldd?"
    echo "$0: Paths: |------------------------------------------ "
    echo "$0: Paths: | "
    echo "$0: Paths: | Based on a query to ldconfig, it appears that your"
    echo "$0: Paths: | GPSTk library install path is NOT in a location known to ldd"
    echo "$0: Paths: | Anything trying to link against libgpstk.so will fail"
    echo "$0: Paths: |  including the GPSTk python bindings."
    echo "$0: Paths: | "
    echo "$0: Paths: | What should be found:"
    echo "$0: Paths: |     gpstk_install_path = $gpstk_install" 
    echo "$0: Paths: |     gpstk_lib_install_path = $gpstk_install/lib" 
    echo "$0: Paths: | "
    echo "$0: Paths: | Where they should be found:"
    echo "$0: Paths: |     ldd_path_list:" 
    for ldd_path_entry in $ldd_path_list; do
    echo "$0: Paths: |         ldd_path_entry = $ldd_path_entry" 
    done;
    echo "$0: Paths: | "
    echo "$0: Paths: |     ld_lib_path_list:" 
    for ld_lib_path_entry in $ld_lib_path_list; do
    echo "$0: Paths: |         ld_lib_path_entry = $ld_lib_path_entry" 
    done;
    echo "$0: Paths: | "
    echo "$0: Paths: | Recommendation: update your environment as follows:"
    echo "$0: Paths: | "
    echo "$0: Paths: |      $ export LD_LIBRARY_PATH=$gpstk_install/lib:\$LD_LIBRARY_PATH "
    echo "$0: Paths: | "
    echo "$0: Paths: |------------------------------------------ "
    echo ""
else
    echo ""
    echo "$0: Paths: |------------------------------------------ "
    echo "$0: Paths: | [PASS]: Post-install config test: Can $gpstk_install/lib be found by ldd?"
    echo "$0: Paths: |------------------------------------------ "
    echo "$0: Paths: | GPSTk library install path appears to be in your PATH"
    echo "$0: Paths: | No further action needed."
    echo "$0: Paths: |------------------------------------------ "
    echo ""
fi

#----------------------------------------
# Test Python Module search path, sys.path
#----------------------------------------

if [ "$build_python" ]; then

    # get the contents of sys.path, the python module search path
    sys_path_raw=`$python_exe -c 'import sys;print(sys.path)'`

    # filter the square brackets, commas, and quotes and put one path on each line
    sys_path_list=`echo $sys_path_raw | sed 's/\(\[\|\]\)//g' | sed "s/'//g" | sed -e 's/,/ /g'`

    # test the sys_path_list to see if it contains the path where this script installed GPSTk python module
    sys_path_test=`echo "$sys_path_list" | grep -o "$python_install"`

    # if the path is in sys.path, then sys_path_test will NOT be empty
    if [ -z "$sys_path_test" ]; then

        echo ""
        echo "$0: Paths: |------------------------------------------ "
        echo "$0: Paths: | [FAIL]: Post-install config test: Can $python_install be found by sys.path?"
        echo "$0: Paths: |------------------------------------------ "
        echo "$0: Paths: | "
        echo "$0: Paths: | Based on a query to $python_exe and sys.path, your "
        echo "$0: Paths: | GPSTk python package install path is not known to sys.path"
        echo "$0: Paths: | Any attempt to 'import gpstk' will fail"
        echo "$0: Paths: | "
        echo "$0: Paths: | What should be found:"
        echo "$0: Paths: |     python_install_path = $python_install" 
        echo "$0: Paths: | "
        echo "$0: Paths: | Where it should be found:"
        echo "$0: Paths: |     sys_path_list:" 
        for sys_path_entry in $sys_path_list; do
            echo "$0: Paths: |          sys_path_entry = $sys_path_entry" 
        done;
        echo "$0: Paths: | "
        echo "$0: Paths: | Recommendation: update your environment as follows:"
        echo "$0: Paths: | "
        echo "$0: Paths: |     $ export PYTHONPATH=$install_prefix/lib/python2.7/site-packages:\$PYTHONPATH "
        echo "$0: Paths: | "
        echo "$0: Paths: |------------------------------------------ "
        echo ""

    else
        echo ""
        echo "$0: Paths: |------------------------------------------ "
        echo "$0: Paths: | [PASS]: Post-install config test: Can $python_install be found by sys.path?"
        echo "$0: Paths: |------------------------------------------ "
        echo "$0: Paths: | Based on a query to $python_exe and sys.path, your "
        echo "$0: Paths: | GPSTk python package install path is known to sys.path"
        echo "$0: Paths: | GPSTk python package 'import gpstk' should work just fine."
        echo "$0: Paths: | No further action needed."
        echo "$0: Paths: |------------------------------------------ "
        echo ""

    fi

fi
#----------------------------------------
# The End
#----------------------------------------
echo ""
echo ""
