all: octave python


srcdir=../../../src

# This rule is a hack since the Jamrules don't make this target automagically
# It probably doesn't get executed when the make build is use
$(srcdir)/libgpstk.so: $(srcdir)/libgpstk.so.16.0
	ln $< $@


# Octave -------------------------------------------------------

octave : gpstk.oct

octave_clean:
	rm -f gpstk.oct octave_wrap.o octave_wrap.cxx

gpstk.oct : octave_wrap.o #$(srcdir)/libgpstk.so
	gcc -shared  octave_wrap.o -o gpstk.oct -L$(srcdir) -lgpstk

octave_wrap.o : octave_wrap.cxx
	g++ -g -c -fpic octave_wrap.cxx

octave_wrap.cxx : gpstk.i
	swig -c++ -octave -o octave_wrap.cxx gpstk.i


# Python ------------------------------------------------------

python : _gpstk.so

# need to find a good way to locate the proper location to install these to.
pysite=/usr/lib/python2.6/site-packages
pysite=/usr/local/lib/python2.6/site-packages
python_install:
	cp _gpstk.so gpstk.py $(pysite)

python_uninstall:
	rm $(pysite)/_gpstk.so $(pysite)/gpstk.py

python_clean:
	rm -f _gpstk.so gpstk.py gpstk.pyc python_wrap.o python_wrap.cxx

_gpstk.so : python_wrap.o #$(srcdir)/libgpstk.so
	gcc -shared `python-config --ldflags` python_wrap.o -L$(srcdir) -lgpstk -o _gpstk.so

ignores=-Wno-unknown-pragmas
python_wrap.o : python_wrap.cxx
	g++ `python-config --cflags` $(ignores) -fpic -c -I$(srcdir) python_wrap.cxx

python_wrap.cxx gpstk.py: gpstk.i
	swig -c++ -python -o python_wrap.cxx gpstk.i

# Perl5 -------------------------------------------------------

gpstk_perl : gpstk.so

gpstk.so : gpstk_perl_wrap.o
	g++ -shared gpstk_perl_wrap.o -L $(srcdir) -lgpstk -o gpstk.so -lgpstk

gpstk_perl_wrap.o : gpstk_perl_wrap.cxx
	g++ -c gpstk_perl_wrap.cxx -I /usr/lib/perl/5.10/CORE  -I $(srcdir)

gpstk_perl_wrap.cxx gpstk.pm: gpstk.i
	swig -c++ -perl -o gpstk_perl_wrap.cxx gpstk.i

# General targets ----------------------------------------------

clean : python_clean octave_clean
	rm -f gpstk.so gpstk.pm # perl cleanup
