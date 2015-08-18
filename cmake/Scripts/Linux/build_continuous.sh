#!/bin/bash
cd ~/git/cdash/continuous/Linux/gpstk
ctest -S ~/git/cdash/continuous/Linux/gpstk/cmake/Scripts/Linux/build_continuous.ctest -V -O continuous.log