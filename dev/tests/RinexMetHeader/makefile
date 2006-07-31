# CppUnit-Tutorial
# file: makefile
# next line has to be changed to the installation-path of CppUnit
CPPUNIT_PATH=/home/ehagen

xRinexMetHeader.tst: xRinexMetHeaderM.o xRinexMetHeader.o RinexMetHeader.o
	g++ -o xRinexMetHeader.tst xRinexMetHeaderM.o xRinexMetHeader.o RinexMetHeader.o -L${CPPUNIT_PATH}/lib -I/home/ehagen/gpstk/src -L/home/ehagen/gpstk/src -lstdc++ -lcppunit -ldl -lgpstk -fprofile-arcs -ftest-coverage

RinexMetHeader.o: RinexMetHeader.cpp RinexMetHeader.hpp
	g++ -c RinexMetHeader.cpp -I/home/ehagen/gpstk/src -L/home/ehagen/gpstk/src -fprofile-arcs -ftest-coverage

xRinexMetHeader.o: xRinexMetHeader.cpp xRinexMetHeader.hpp
	g++ -c xRinexMetHeader.cpp -I${CPPUNIT_PATH}/include -fprofile-arcs -ftest-coverage

xRinexMetHeaderM.o: xRinexMetHeaderM.cpp
	g++ -c xRinexMetHeaderM.cpp -I${CPPUNIT_PATH}/include -fprofile-arcs -ftest-coverage

clean:
	rm -f *.o
	rm -f *.g*
	rm -f *.*.g*
