#!/bin/bash
cd ~/git/cdash/continuous/gpstk
ctest -S ~/git/cdash/continuous/gpstk/cmake/Scripts/Windows/build_continuous.ctest -V -O continuous.log