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
#    $ build.sh -h
#
#----------------------------------------

#----------------------------------------
# Qué hora es? Dónde estamos? Y dónde vamos?
#----------------------------------------

source $(dirname "$BASH_SOURCE")/build_setup.sh

user_install_prefix+="/gpstk"
system_install_prefix+="/gpstk"

usage()
{
    cat << EOF
purpose:   This script automates and documents how to build, test, and install the GPSTk.

usage:     $0 [opts] [-- cmake options...]

examples:
   $ build.sh        # Just build software
   $ sudo build.sh -s -b /tmp/qwe    # Build and install core to $system_install_prefix
   $ build.sh -tue     # Build, test and install core, external, and python bindings to $gpstk

OPTIONS:

   -h                   Display this help message.

   -b <build_path>      Specify the cmake build directory to use.

   -i <install_prefix>  Install the build to the given path.

   -j <num_threads>     Number of threads to have make use. Defauts to $num_threads 
                        on this host.

   -c                   Clean out any files in the build dir prior to running cmake.

   -d                   Build documentation, including generate dependency graphs 
                        using GraphViz (.DOT and .PDF files).

   -e                   GPSTk has several parts: core, ext, and python/swig bindings.
                        See README.txt for details.
                        Default (without -e) will build only core
                        Optional (with -e) will build core, ext, and swig

   -u                   Install the build to the path in the \$gpstk environment variable.
                        If this variable is not set, it will be installed to
                        $user_install_prefix.

   -s                   Install the build into $system_install_prefix and the python 
                        bindings to the default system location. Make sure the build path 
                        is writable by root.

   -x                   Disable building the python bindings. Default is to build them
                        if -e is specified.

   -P  <python_exe>     Python executable used to help determine with python system libraries
                        will be used when building python extension package. 
                        Default=$python_exe

   -t                   Build and run tests.

   -p                   Build supported packages (source, binary, deb,  ...)

   -v                   Include debugging output.

Notes:
   Remember to add user and non-standard locations to your environment. Eg:
     $ export LD_LIBRARY_PATH=/tmp/test/gpstk/lib:\$LD_LIBRARY_PATH
     $ export PYTHONPATH=/tmp/test/lib/python2.7/site-packages:\$PYTHONPATH

EOF
}


while getopts "hb:cdepi:j:xP:sutv" OPTION; do
    case $OPTION in
        h) usage
           exit 0
           ;;
        b) build_root=$(abspath ${OPTARG})
           ;;
        c) clean=1
           ;;
        d) build_docs=1
           ;;
        e) build_ext=1
           build_python=1
           ;;
        p) build_packages=1
           ;;
        i) install=1
           install_prefix=$(abspath ${OPTARG})
           ;;
        j) num_threads=$OPTARG
           ;;
        x) exclude_python=1
           ;;
        P) python_exe=$OPTARG
           ;;
        u) install=1
           install_prefix=${gpstk:-$user_install_prefix}
           python_install=$user_python_install
           ;;
        s) install=1
           install_prefix=$system_install_prefix
           python_install=$system_python_install
           ;;
        t) test_switch=1
           ;;
        v) verbose=1
           ;;
        *) echo "Invalid option: -$OPTARG" >&2
           usage
           exit 2
           ;;
    esac
done

shift $(($OPTIND - 1))
LOG=$build_root/build.log


#----------------------------------------
# Clean build directory
#----------------------------------------
if [ ! -d "$build_root" ]; then
    mkdir -p "$build_root"
fi

if [ $clean ]; then
    rm -rf $build_root/*
    log "Cleaned out $build_root ..."
fi

log "============================================================"
log "GPSTk build config ..."
log "repo            = $repo"
log "build_root      = $build_root"
log "install         = $(ptof $install)"
log "install_prefix  = $install_prefix"
log "build_ext       = $(ptof $build_ext)"
log "exclude_python  = $(ptof $exclude_python)"
log "python_install  = $python_install"
log "python_exe      = $python_exe"
log "build_docs      = $(ptof $build_docs)"
log "build_packages  = $(ptof $build_packages)"
log "test_switch     = $(ptof $test_switch)"
log "clean           = $(ptof $clean)"
log "verbose         = $(ptof $verbose)"
log "num_threads     = $num_threads"
log "cmake args      = $@"
log "time            =" `date`
log "hostname        =" $hostname
log "uname           =" `uname -a`
log "git branch      =" $git_branch
log "logfile         =" $LOG
log


cd $build_root

if [ $build_docs ]; then
    log "Pre-build documentation processing ..."
    # Dynamically configure the Doxyfile with the source and destination paths
    sources="$repo/core/lib"
    if [ $build_ext ]; then
        sources+=" $repo/ext/lib"
    fi
    log "Generating dOxygen files from C/C++ source ..."
    sed -e "s#gpstk_sources#$sources#g" -e "s#gpstk_doc_dir#$build_root/doc#g" $repo/Doxyfile | doxygen - >$build_root/dOxygen.log
    tar -czf gpstk_doc_cpp.tgz -C doc/html .
    
    if [[ -z $exclude_python && $build_ext ]] ; then
        log "Generating swig/python doc files from dOxygen output ..."
        ${python_exe} $repo/swig/docstring_generator.py $build_root/doc $build_root/swig/doc >$build_root/swig_doc.log
    fi
fi

# setup the cmake command
args=$@
if [ $exclude_python ]; then
    args+=" -DBUILD_PYTHON=OFF"
elif [ $build_ext ]; then
    args+=" -DBUILD_PYTHON=ON"
    args+=${python_exe:+" -DPYTHON_EXECUTABLE=$python_exe"}
    args+=${python_install:+" -DPYTHON_INSTALL_PREFIX=$python_install"}
fi
args+=${install_prefix:+" -DCMAKE_INSTALL_PREFIX=$install_prefix"}
args+=${build_ext:+" -DBUILD_EXT=ON"}
args+=${verbose:+" -DDEBUG_SWITCH=ON"}
args+=${test_switch:+" -DTEST_SWITCH=ON"}
args+=${build_docs:+" --graphviz=$build_root/doc/graphviz/gpstk_graphviz.dot"}

exit_on_fail=1

run cmake $args $repo

run make all -j $num_threads

exit_on_fail=0
if [ $test_switch ]; then
    run ctest -v -j $num_threads
    log "See $build_root/Testing/Temporary/LastTest.log for detailed results"
fi

if [ $install ]; then
    run make install -j $num_threads
fi

if [ $build_docs ]; then
    log "Post-build documentation processing ..."
    # This is commented out because the RST documentation polutes the repo at the moment
    # This process needs to be re-factored to use the CMAKE_CURRENT_BINARY_DIR
#    if [[ -z $exclude_python && $build_ext ]] ; then
#        log "Building RST documentation with Sphinx ..."
#        cd $repo/swig/sphinx
#        make html
#        tar -czf $build_root/gpstk_doc_python.tgz -C $repo/swig/sphinx/_build/html/ .
#    fi

    log "Generating GraphViz output PDF ..."
    dot -Tpdf $build_root/doc/graphviz/gpstk_graphviz.dot -o $build_root/doc/graphviz/gpstk_graphviz.pdf
fi

if [ $build_packages ]; then
    run make package
    run make package_source
    
    if [[ -z $exclude_python && $build_ext ]] ; then
        cd $build_root/swig/install_package
        ${python_exe} setup.py sdist --formats=zip,gztar
    fi
fi

log
log "GPSTk build done. :-)"
log `date`
