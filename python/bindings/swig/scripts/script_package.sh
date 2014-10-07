#!/bin/bash

build() {
	
	PACKAGE_PATH=$HOME/git/gpstk/python/bindings/swig/install_package
	cd $PACKAGE_PATH
    python setup.py sdist --formats=zip,gztar
    cd dist
    py2dsc gpstk-2.5.tar.gz
    cd deb_dist/gpstk-2.5/
    dpkg-buildpackage -rfakeroot -uc -us
}

build
