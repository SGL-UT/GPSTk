all: gpstk_octave #gpstk_python  gpstk_perl

gpstk_octave : gpstk.oct

# Octave -------------------------------------------------------

gpstk.oct : gpstk_octave_wrap.o
	gcc -shared  gpstk_octave_wrap.o -o gpstk.oct -lgpstk

gpstk_octave_wrap.o : gpstk_octave_wrap.cxx
	g++ -g -c -fpic -lgpstk -lm gpstk_octave_wrap.cxx -I /usr/include

gpstk_octave_wrap.cxx : gpstk.i
	swig -c++ -octave -o gpstk_octave_wrap.cxx gpstk.i

# Python ------------------------------------------------------

gpstk_python : _gpstk.so

_gpstk.so : gpstk_python_wrap.o
	g++ -shared `python-config --ldflags` gpstk_python_wrap.o -lgpstk -o _gpstk.so

gpstk_python_wrap.o : gpstk_python_wrap.cxx
	g++ `python-config --cflags` -fPIC -c gpstk_python_wrap.cxx

gpstk_python_wrap.cxx : gpstk.i
	swig -c++ -python -o gpstk_python_wrap.cxx gpstk.i

# Perl5 -------------------------------------------------------

gpstk_perl : gpstk.so

gpstk.so : gpstk_perl_wrap.o
	g++ -shared gpstk_perl_wrap.o -lgpstk -o gpstk.so -lgpstk

gpstk_perl_wrap.o : gpstk_perl_wrap.cxx
	g++ -c gpstk_perl_wrap.cxx -I /usr/lib/perl/5.10/CORE  -I /usr/local/include/gpstk

gpstk_perl_wrap.cxx : gpstk.i
	swig -c++ -perl -o gpstk_perl_wrap.cxx gpstk.i

# General targets ----------------------------------------------

clean :
	rm -f *.o *.oct *_wrap.cxx *_wrap.c 
	rm -f _gpstk.so # python cleanup
	rm -f gpstk.so # perl cleanup
