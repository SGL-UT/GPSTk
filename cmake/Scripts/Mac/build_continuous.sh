#!/bin/bash
cd ~/git/cdash/continuous/gpstk
ctest -S ~/git/cdash/continuous/gpstk/cmake/Scripts/Mac/build_continuous.ctest -V -O continuous.log