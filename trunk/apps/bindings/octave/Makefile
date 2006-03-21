#
# $Id: //depot/sgl/gpstk/dev/apps/bindings/octave/Makefile#1 $
#
# To use these routines the GPSTk shared library either needs to be
# installed as a system shared library, or the environment variable
# LD_LIBRARY_PATH needs to set to the directory where the shared 
# library file resides
#

all :	readRinexObsFast.oct readRinexObsGeom.oct \
	calculatePosition.oct

readRinexObsFast.oct: readRinexObsFast.cpp
	$(make_oct)

readRinexObsGeom.oct: readRinexObsGeom.cpp
	$(make_oct)

calculatePosition.oct: calculatePosition.cpp
	$(make_oct)

define make_oct
	@echo "making oct file: $@"
	@echo "mkoctfile $(CXXFLAGS) -o $@ $^ $(LDLIBS)"
	mkoctfile -I../src -L../src -lgpstk -o $@ $^ $(LDLIBS)
	chmod 755 $@
	for file in $^ ; do $(RM) $$file.o ; done  
endef

