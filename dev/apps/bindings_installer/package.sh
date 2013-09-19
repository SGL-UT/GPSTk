#!/bin/bash

build() {
    cd ../.. # in gpstk/dev
    # doxygen

    cd apps/bindings/swig
    # python docstring_generator.py

    cd bin
    rm *.cxx
    cmake ..
    make -j6
    python gpstk_builder.py .

    mkdir ../../../bindings_installer/gpstk
    mv -f gpstk_pylibPYTHON_wrap.cxx ../../../bindings_installer/gpstk
    mv -f gpstk_pylibPYTHON_wrap.h ../../../bindings_installer/gpstk
    mv -f gpstk/gpstk_pylib.py ../../../bindings_installer/gpstk
    mv -f gpstk/__init__.py ../../../bindings_installer/gpstk
    mv -f gpstk/cpp ../../../bindings_installer/gpstk
    mv -f gpstk/constants ../../../bindings_installer/gpstk
    mv -f gpstk/exceptions ../../../bindings_installer/gpstk

    cd ../../../.. # in gpstk/dev
    python setup.py sdist --formats=zip,gztar

    cd apps/bindings/swig/sphinx
    make html
    cd _build/html
    zip ../../../../../../dist/gpstkpythondoc.zip *


    cd ../../../../../.. # in gpstk/dev

    cd dist
    py2dsc gpstk-2.2.tar.gz
    cd deb_dist/gpstk-2.2/
    dpkg-buildpackage -rfakeroot -uc -us
}

build
