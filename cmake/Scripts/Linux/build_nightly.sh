#!/bin/bash
cd ~/git/cdash/nightly/Linux/gpstk
ctest -S ~/git/cdash/nightly/Linux/gpstk/cmake/Scripts/Linux/build_nightly.ctest -V -O nightly.log