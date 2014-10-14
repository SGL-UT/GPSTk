# CppUnit-Tutorial
# file: makefile
# next line has to be changed to the installation-path of CppUnit
CPPUNIT_PATH=/home/ehagen

xIonoModel.tst: xIonoModelM.o xIonoModel.o IonoModel.o
	g++ -o xIonoModel.tst xIonoModelM.o xIonoModel.o IonoModel.o -L${CPPUNIT_PATH}/lib -lstdc++ -lcppunit -ldl -l gpstk -ftest-coverage -fprofile-arcs


IonoModel.o: IonoModel.cpp IonoModel.hpp
	g++ -c IonoModel.cpp -I/home/ehagen/gpstk/src -L/home/ehagen/gpstk/src -ftest-coverage -fprofile-arcs

xIonoModel.o: xIonoModel.cpp xIonoModel.hpp
	g++ -c xIonoModel.cpp -I${CPPUNIT_PATH}/include -I/home/ehagen/gpstk/src -L/home/ehagen/gpstk/src -ftest-coverage -fprofile-arcs

xIonoModelM.o: xIonoModelM.cpp
	g++ -c xIonoModelM.cpp -I${CPPUNIT_PATH}/include -ftest-coverage -fprofile-arcs

clean:
	rm -f *.o
	rm -f *.g*
	rm -f *.*.g*
