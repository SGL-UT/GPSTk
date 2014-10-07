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

cd $my_package_path
python setup.py install --prefix=~/.local

echo " "
echo " "
echo " "
echo " "
echo " Install results ..."
echo " "
echo " "
echo " "
echo " "
echo " "

echo "=================================================================="
echo "Contents of ${my_install_path}/lib/python2.7/site-packages/gpstk"
echo "=================================================================="
ls -l ${my_install_path}/lib/python2.7/site-packages/gpstk

echo "=================================================================="
echo "Contents of ${my_install_path}/lib/python2.7/site-packages/gpstk/test"
echo "=================================================================="
ls -l ${my_install_path}/lib/python2.7/site-packages/gpstk/test

echo "=================================================================="
echo "Contents of ${my_install_path}/lib/python2.7/site-packages/gpstk/data"
echo "=================================================================="
ls -l ${my_install_path}/lib/python2.7/site-packages/gpstk/data

echo "=================================================================="
echo "Contents of $HOME/.local"
echo "=================================================================="
ls -l ${my_install_path}

echo "=================================================================="
echo "Contents of $HOME/.local/bin"
echo "=================================================================="
ls -l ${my_install_path}/bin

echo "=================================================================="
echo "Contents of $HOME/.local/data"
echo "=================================================================="
ls -l ${my_install_path}/data

echo "=================================================================="
echo "Contents of $HOME/.local/gpstk"
echo "=================================================================="
ls -l ${my_install_path}/gpstk

