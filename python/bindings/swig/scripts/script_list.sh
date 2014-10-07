#!/bin/bash

my_package_path=$HOME/git/gpstk/python/bindings/swig/install_package
my_install_path=$HOME/.local

cd $my_package_path

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

echo "=================================================================="
echo "Contents of $HOME/.local/gpstk/data"
echo "=================================================================="
ls -l ${my_install_path}/gpstk/data

