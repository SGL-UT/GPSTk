# file: makefile
# next line has to be changed to the installation-path of CppUnit
CPPUNIT_PATH=/home/ehagen

xBCEphemerisStore.tst: xRinexEphemerisStoreM.o xRinexEphemerisStore.o RinexEphemerisStore.o BCEphemerisStore.o
	g++ -o xBCEphemerisStore.tst xRinexEphemerisStoreM.o xRinexEphemerisStore.o RinexEphemerisStore.o BCEphemerisStore.o -L${CPPUNIT_PATH}/lib -lstdc++ -lcppunit -ldl -lgpstk -fprofile-arcs -ftest-coverage

RinexEphemerisStore.o: RinexEphemerisStore.cpp RinexEphemerisStore.hpp
	g++ -c RinexEphemerisStore.cpp -I/home/ehagen/gpstk/src -L/home/ehagen/gpstk/src -lgpstk -fprofile-arcs -ftest-coverage
xRinexEphemerisStore.o: xRinexEphemerisStore.cpp xRinexEphemerisStore.hpp
	g++ -c xRinexEphemerisStore.cpp -I${CPPUNIT_PATH}/include -I/home/ehagen/gpstk/src -L/home/ehagen/gpstk/src -fprofile-arcs -ftest-coverage

xRinexEphemerisStoreM.o: xRinexEphemerisStoreM.cpp
	g++ -c xRinexEphemerisStoreM.cpp -I${CPPUNIT_PATH}/include -fprofile-arcs -ftest-coverage

BCEphemerisStore.o: BCEphemerisStore.cpp BCEphemerisStore.hpp
	g++ -c BCEphemerisStore.cpp -I/home/ehagen/gpstk/src -L/home/ehagen/gpstk/src -lgpstk -fprofile-arcs -ftest-coverage


clean:
	rm -f *.o
	rm -f *.txt
	rm -f *.g*
	rm -f *.*.g*
