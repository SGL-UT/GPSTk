#
# This simple makefile builds a Python interface to the GPSTk from code
# generated the SWIG utility (http://www.swig.org/). 
#
# You shouldn't need to install SWIG to compile this interface as
# distributed, unless you change the interface definition (.i files).
#
# What you do need:
#  - A C/C++ compiler.
#  - Development headers of Python
# 
# Not all GPSTk functionality is present in these bindings.
# Two reasons: First, this is a work in progress and should be considered
# incomplete. Second, not all of the GPSTk's functionality _can_ be mapped
# into other languages such as Python.
#
# Examples of usage of this interface are in the examples subdirectory.
#
# Reminder for later: $@ is target, $< is rhs, $^ is rhs
#
# See Developer.txt for more info about how to further extend this interface.
# Note that swig version 1.3.25 and gpstk are both required to be installed
# for this makefile to work.
#

all :	_gpstkPython.so gpstkPython.html

gpstkPython_wrap.cxx: common.i ExtraWaveFact.i gpstkPythonUtils.i RinexObsType.i DayTimeException.i gpstkPython.i RinexDatum.i sensorType.i streamRead.i gpstkPythonUtils.cpp streamRead.cpp
	swig -python -c++ -I/usr/local/lib gpstkPython.i

gpstkPython_wrap.o: gpstkPython_wrap.cxx
	g++ -c gpstkPython_wrap.cxx -I/usr/include/python2.3

_gpstkPython.so: gpstkPython_wrap.o
	g++ -shared gpstkPython_wrap.o -o _gpstkPython.so -lgpstk

gpstkPython.html: _gpstkPython.so
	./pydoc.py -w gpstkPython

clean:
	rm gpstkPython.py*
	rm *_wrap*
	rm *gpstk*.so
	rm gpstkPython.html
