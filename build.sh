#!/bin/bash
#----------------------------------------
#
# Purpose: GPSTk build and install script
#
#     Automate the use of CMake, SWIG, Doxygen, Sphinx, and distutils
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

# or set gpstk=~/.local/gpstk
#user_install_prefix+="/gpstk"
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
   $ build.sh -vt  -- -DCMAKE_BUILD_TYPE=debug   # build for running debugger
   $ build.sh -vt  -- -DCMAKE_BUILD_TYPE=release # build for release
   # MUST choose release to get -O3 optimization on linux

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

   -n                   Do not use address sanitizer for debug build (used by default)

   -P  <python_exe>     Python executable used to help determine with python system libraries
                        will be used when building python extension package.
                        Default=$python_exe

   -t                   Build and run tests.
   -T                   Build and run tests but don't stop on test failures.

   -g                   Compile code with gcov instrumenation enabled.

   -p                   Build supported packages (source, binary, deb,  ...)

   -v                   Include debugging output.
EOF
}


while getopts "hb:cdepi:j:xnP:sutTgv" OPTION; do
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
           python_install=$install_prefix
           ;;
        j) num_threads=$OPTARG
           ;;
        x) exclude_python=1
           ;;
        n) no_address_sanitizer=1
           ;;
        P) python_exe=$OPTARG
           ;;
        u) install=1
           install_prefix=${gpstk:-$user_install_prefix}
           python_install=$user_python_install
           user_install=1
           ;;
        s) install=1
           install_prefix=$system_install_prefix
           python_install=$system_python_install
           ;;
        t) test_switch=1
           ;;
        T) test_switch=-1
           ;;
        g) coverage_switch=1
           ;;
        v) verbose+=1
           ;;
        *) echo "Invalid option: -$OPTARG" >&2
           usage
           exit 2
           ;;
    esac
done

shift $(($OPTIND - 1))
LOG="$build_root"/build.log

#----------------------------------------
# Clean build directory
#----------------------------------------
if [ ! -d "$build_root" ]; then
    mkdir -p "$build_root"
fi

if [ -f "$LOG" ]; then
    rm $LOG
fi

if [ $clean ]; then
    rm -rf "$build_root"/*
    log "Cleaned out $build_root ..."
fi

if ((verbose>0)); then
    log "============================================================"
    log "GPSTk build config ..."
    log "repo                 = $repo"
    log "build_root           = $build_root"
    log "install              = $(ptof $install)"
    log "install_prefix       = $install_prefix"
    log "build_ext            = $(ptof $build_ext)"
    log "exclude_python       = $(ptof $exclude_python)"
    log "python_install       = $python_install"
    log "python_exe           = $python_exe"
    log "no_address_sanitizer = $(ptof $no_address_sanitizer)"
    log "build_docs           = $(ptof $build_docs)"
    log "build_packages       = $(ptof $build_packages)"
    log "test_switch          = $(ptof $test_switch)"
    log "coverage_switch = $(ptof $coverage_switch)"
    log "clean                = $(ptof $clean)"
    log "verbose              = $(ptof $verbose)"
    log "num_threads          = $num_threads"
    log "cmake args           = $@"
    log "time                 =" `date`
    log "hostname             =" $hostname
    log "uname                =" `uname -a`
    log "git id               =" $(get_repo_state $repo)
    log "logfile              =" $LOG
    log
fi

if ((verbose>2)); then
    log "============================================================"
    set >> $LOG
    log "============================================================"
fi

if ((verbose>3)); then
    exit
fi

# Doxygen should be run in the top level directory so it picks up
# formatting files
cd "$repo"
if [ $build_docs ]; then
    log "Pre-build documentation processing ..."
    # Dynamically configure the Doxyfile with the source and destination paths
    sources="$repo/core/lib"
    if [ $build_ext ]; then
        sources+=" $repo/ext/lib"
    fi
    log "Generating Doxygen files from C/C++ source ..."
    sed -e "s#^INPUT *=.*#INPUT = $sources#" -e "s#gpstk_sources#$sources#g" -e "s#gpstk_doc_dir#$build_root/doc#g" $repo/Doxyfile >$repo/doxyfoo
    sed -e "s#^INPUT *=.*#INPUT = $sources#" -e "s#gpstk_sources#$sources#g" -e "s#gpstk_doc_dir#$build_root/doc#g" $repo/Doxyfile | doxygen - >"$build_root"/Doxygen.log
    tar -czf gpstk_doc_cpp.tgz -C "$build_root"/doc/html .

    if [[ -z $exclude_python && $build_ext ]] ; then
        log "Generating swig/python doc files from Doxygen output ..."
        ${python_exe} $repo/swig/docstring_generator.py "$build_root"/doc "$build_root"/swig/doc >"$build_root"/swig_doc.log
    fi
fi

cd "$build_root"

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
args+=${user_install:+" -DPYTHON_USER_INSTALL=ON"}
args+=${test_switch:+" -DTEST_SWITCH=ON"}
args+=${coverage_switch:+" -DCOVERAGE_SWITCH=ON"}
args+=${build_docs:+" --graphviz=$build_root/doc/graphviz/gpstk_graphviz.dot"}
if [ $no_address_sanitizer ]; then
    args+=" -DADDRESS_SANITIZER=OFF"
else
    args+=" -DADDRESS_SANITIZER=ON"
fi

case `uname` in
    MINGW32_NT-6.1)
        run cmake $args -G "Visual Studio 14 2015 Win64" $repo
        run cmake --build . --config Release
        ;;
    MINGW64_NT-6.3)
        run cmake $args -G "Visual Studio 14 2015 Win64" $repo
        run cmake --build . --config Release
        ;;
    *)
        echo "Run cmake $args $repo ##########################"
        run cmake $args $repo
        run make all -j $num_threads
        #run make all -j $num_threads VERBOSE=1   # BWT make make verbose
esac


if [ $test_switch ]; then
  if ((test_switch < 0)); then
      ignore_failures=1
  fi
  case `uname` in
      MINGW32_NT-6.1)
          run cmake --build . --target RUN_TESTS --config Release
          ;;
      MINGW64_NT-6.3)
          run cmake --build . --target RUN_TESTS --config Release
          ;;
      *)
          run ctest -v -j $num_threads
          test_status=$?
  esac
  unset ignore_failures
fi

if [ $install ]; then
    case `uname` in
    MINGW32_NT-6.1)
        run cmake --build . --config Release --target install
        ;;
    MINGW64_NT-6.3)
        run cmake --build . --config Release --target install
        ;;
    *)
        run make install -j $num_threads
    esac
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
    dot -Tpdf "$build_root"/doc/graphviz/gpstk_graphviz.dot -o "$build_root"/doc/graphviz/gpstk_graphviz.pdf
fi

if [ $build_packages ]; then
    case `uname` in
        MINGW64_NT-6.3)
            run cpack -C Release
            ;;
        MINGW32_NT-6.1)
            run cpack -C Release
            ;;
        *)
            run make package
            run make package_source
    esac
    if [[ -z $exclude_python && $build_ext ]] ; then
        cd "$build_root"/swig/install_package
        ${python_exe} setup.py sdist --formats=zip,gztar
    fi
fi

log
if [ $test_switch ]; then
    if [ $test_status == 0 ]; then
        log "All tests passed!"
    else
        log $test_status " test failures."
    fi
else
    log "Tests not run."
fi
log "See $build_root/Testing/Temporary/LastTest.log for detailed test log"
log "See $LOG for detailed build log"
log
log "GPSTk build done. :-)"
log `date`
