#!/bin/bash

my_package_path=$HOME/git/gpstk/python/bindings/swig/install_package
my_install_path=$HOME/.local
rm -rf $my_install_path/bin/*
rm -rf $my_install_path/data
rm -rf $my_install_path/gpstk
rm -rf $my_install_path/lib/python2.7/site-packages/gpstk
rm -rf $my_package_path/build
rm -rf $my_package_path/gpstk.egg-info
rm -rf $my_package_path/dist
