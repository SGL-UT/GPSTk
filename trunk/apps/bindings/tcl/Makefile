#
# $Id:$
#
# This simple makefile builds a Tcl/Tk interface to the GPSTk from code
# generated the SWIG utility (http://www.swig.org/). 
#
# You shouldn't need to install SWIG to compile this interface as
# distributed, unless you change the interface definition (.i files).
#
# What you do need:
#  - A C/C++ compiler.
#  - Development headers of Tcl/Tk
#  - Knowledge where the header tcl.h is installed
#  - The sed utility
# 
# Not all GPSTk functionality is present in these Tcl/Tk bindings.
# Two reasons: First, this is a work in progress and should be considered
# incomplete. Second, not all of the GPSTk's functionality _can_ be mapped
# into other languages such as Tcl/Tk.
#
# Examples of usage of this interface are in the examples subdirectory.
#
# Reminder for later: $@ is target, $< is rhs, $^ is rhs

all :	gpstk-tcl.so 

gpstk_wrap.cxx: ../gpstk.i ../common.i ../Exception.i ../GPSZcount.i \
	../DayTime.i ../FFTextStream.i ../RinexObsStream.i
	swig -tcl -c++ -importall -ignoremissing -I.. \
	gpstk.i

gpstk_wrap_mod.cxx: gpstk_wrap.cxx
	sed 's/DayTime.hpp\"/DayTime.hpp\"\nusing namespace gpstk;/' gpstk_wrap.cxx > gpstk_wrap_mod.cxx

gpstk_wrap_mod.o: gpstk_wrap_mod.cxx
	g++ -fpic -c gpstk_wrap_mod.cxx -I.. -I /usr/include/tcl8.4

gpstk-tcl.so: gpstk_wrap_mod.o
	g++ -shared gpstk_wrap_mod.o -o gpstk-tcl.so -lgpstk

clean:
	rm *_wrap*
	rm *gpstk*.so






