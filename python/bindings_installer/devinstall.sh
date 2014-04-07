#!/bin/bash

build() {
    cd ../../dev/ # in dev
    doxygen
    cd ~/git/gpstk/python/bindings/swig
    python docstring_generator.py
    cd bin
    cmake ..
    make -j4
    python gpstk_builder.py  ~/.local/lib/python2.7/site-packages/
}

build
