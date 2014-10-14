# CppUnit-Tutorial
# file: makefile
# next line has to be changed to the installation-path of CppUnit
CPPUNIT_PATH=/home/ehagen

xPolyFit.tst: xPolyFitM.o xPolyFit.o PolyFit.o
	g++ -o xPolyFit.tst xPolyFitM.o xPolyFit.o PolyFit.o -L${CPPUNIT_PATH}/lib -lstdc++ -lcppunit -ldl -lgpstk -fprofile-arcs -ftest-coverage


PolyFit.o: PolyFit.cpp PolyFit.hpp
	g++ -c PolyFit.cpp -I/home/ehagen/gpstk/src -L/home/ehagen/gpstk/src -fprofile-arcs -ftest-coverage

xPolyFit.o: xPolyFit.cpp xPolyFit.hpp
	g++ -c xPolyFit.cpp -I${CPPUNIT_PATH}/include -I/home/ehagen/gpstk/src -L/home/ehagen/gpstk/src -fprofile-arcs -ftest-coverage

xPolyFitM.o: xPolyFitM.cpp
	g++ -c xPolyFitM.cpp -I${CPPUNIT_PATH}/include -fprofile-arcs -ftest-coverage

clean:
	rm -f *.o
	rm -f *.g*
	rm -f *.*.g*
