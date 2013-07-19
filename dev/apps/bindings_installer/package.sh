#! /bin/bash

build() {
    cd ../..
    doxygen

    cd apps/bindings/swig
    python docstring_generator.py

    cd ../bindings/swig/bin
    rm ../bindings/swig/bin/*.cxx
    cmake ..
    make -j4
    python gpstk_builder.py .

    mkdir ../../../bindings_installer/gpstk
    mv gpstk_pylibPYTHON_wrap.cxx ../../../bindings_installer/gpstk
    mv gpstk/gpstk_pylib.py ../../../bindings_installer/gpstk
    mv gpstk/__init__.py ../../../bindings_installer/gpstk
    mv gpstk/cpp ../../../bindings_installer/gpstk
    mv gpstk/constants ../../../bindings_installer/gpstk
    mv gpstk/exceptions ../../../bindings_installer/gpstk

    cd ../../..
    python setup.py sdist --formats=zip,gztar
    python setup.py bdist --formats=gztar
}

build
