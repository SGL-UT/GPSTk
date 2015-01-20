#!/bin/bash
#------------------------------------------------------------
# Purpose: Test script for compiling test applications against
#          a local install of GPSTk while debugging test apps
#          without having to use all the CMake machinations.
#
# Example: $ ./build_test.sh Rinex_Obs_T
#
# Notes:   You must export the path to the non-system install 
#          of libgpstk.so or else the loader will grab the older
#          system installed libgpstk.so at run time, which will
#          result in several unknwon symbol errors on execution.
#          Update your shell from which you want to run the
#          app as follows:
#           $ export LD_LIBRARY_PATH=$GPSTK_INSTALL/lib:$LD_LIBRARY_PATH
#------------------------------------------------------------

FILE_ROOT=$1
INPUT=$FILE_ROOT.cpp
OUTPUT=$FILE_ROOT.exe
GPSTK_INSTALL=$HOME/.local/gpstk
# COMPILER="g++ -g"
COMPILER="gcc -Wall"

COMMAND_STRING="$COMPILER -I$GPSTK_INSTALL/include -L$GPSTK_INSTALL/lib -lgpstk -o $OUTPUT $INPUT"
echo "$COMMAND_STRING"
eval $COMMAND_STRING

