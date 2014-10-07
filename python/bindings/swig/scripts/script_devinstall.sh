#!/bin/bash

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

build() {
	$gpstk_root=$HOME/git/gpstk
	cd $gpstk_root/dev
    doxygen
    cd $gpstk_root/python/bindings/swig
    python docstring_generator.py
    cd bin
    cmake ..
    make -j$num_cores
    cd $gpstk_root/python/bindings/swig/install_package
    python setup.py install --prefix=$HOME/.local
}

build
