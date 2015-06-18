#!/bin/bash
cd ~/git/cdash/nightly/gpstk
ctest -S ~/git/cdash/nightly/gpstk/cmake/Scripts/Mac/build_nightly.ctest -V -O nightly.log