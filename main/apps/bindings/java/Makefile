#
# $Id: //depot/sgl/gpstk/dev/apps/bindings/java/Makefile#1 $
#
# This simple makefile builds a Java interface to the GPSTk from code
# generated the SWIG utility (http://www.swig.org/). 
#
#
# What you do need:
#  - A C/C++ compiler.
#  - Development kit for Java
#  - The sed utility
#  - the swig utility
# 
# Not all GPSTk functionality is present in these bindings.
# Two reasons: First, this is a work in progress and should be considered
# incomplete. Second, not all of the GPSTk's functionality _can_ be mapped
# into other languages such as Java.
#
# Examples of usage of this interface are in the examples subdirectory.
#
# Reminder for later: $@ is target, $< is rhs, $^ is rhs

all :	libgpstk.so 

gpstk_wrap.cxx: ../gpstk.i ../common.i ../Exception.i ../GPSZcount.i \
	../DayTime.i
	swig -java -c++ -noexcept -I.. gpstk.i

gpstk_wrap_mod.cxx: gpstk_wrap.cxx
	sed 's/DayTime.hpp\"/DayTime.hpp\"\nusing namespace gpstk;/' gpstk_wrap.cxx > gpstk_wrap_mod.cxx

gpstk_wrap_mod.o: gpstk_wrap_mod.cxx
	g++ -fPIC -c gpstk_wrap_mod.cxx -I.. \
	-I/usr/local/jdk1.5.0_01/include \
	-I/usr/local/jdk1.5.0_01/include/linux

libgpstk.so: gpstk_wrap_mod.o
	g++ -shared gpstk_wrap_mod.o -o libgpstk.so -lgpstk

clean:
	rm *.class
	rm *.java
	rm *_wrap*
	rm *gpstk*.so







