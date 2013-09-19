#!/bin/bash

build() {
    cd ../.. # in dev
    doxygen
    cd ~/gpstk/dev/apps/bindings/swig
    python docstring_generator.py
    cd bin
    cmake ..
    make -j4
    python gpstk_builder.py  ~/.local/lib/python2.7/site-packages/
}

build
