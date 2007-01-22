#!/bin/sh

export AUTOCONF_VERSION=2.60
export AUTOMAKE_VERSION=1.9

autoreconf

exit
env LDFLAGS="-lstdc++ -lm" CFLAGS="-O0 -fPIC" CXXFLAGS="-O0 -fPIC" ./configure --prefix=/foo/gpstk
