# CppUnit-Tutorial
# file: makefile
# next line has to be changed to the installation-path of CppUnit
CPPUNIT_PATH=/home/ehagen

all: xDayTime.tst

xDayTime.tst: xDayTimeConv.tst xDayTimeInc.tst 

xDayTimeConv.tst: xDayTimeConvM.o xDayTimeConv.o DayTime.o
	g++ -o xDayTimeConv.tst xDayTimeConvM.o xDayTimeConv.o DayTime.o -L${CPPUNIT_PATH}/lib -lstdc++ -lcppunit -ldl -lgpstk -fprofile-arcs -ftest-coverage

xDayTimeInc.tst: xDayTimeIncM.o xDayTimeInc.o DayTime.o
	g++ -o xDayTimeInc.tst xDayTimeIncM.o xDayTimeInc.o DayTime.o -L${CPPUNIT_PATH}/lib -lstdc++ -lcppunit -ldl -lgpstk -fprofile-arcs -ftest-coverage

xDayTimeInc.o: xDayTimeInc.cpp
	g++ -c xDayTimeInc.cpp -I${CPPUNIT_PATH}/include -lgpstk -fprofile-arcs -ftest-coverage

xDayTimeIncM.o: xDayTimeIncM.cpp
	g++ -c xDayTimeIncM.cpp -I${CPPUNIT_PATH}/include -fprofile-arcs -ftest-coverage

DayTime.o: DayTime.cpp DayTime.hpp
	g++ -c DayTime.cpp -I/home/ehagen/gpstk/src -L/home/ehagen/gpstk/src -lgpstk -fprofile-arcs -ftest-coverage

xDayTimeConv.o: xDayTimeConv.cpp
	g++ -c xDayTimeConv.cpp -I${CPPUNIT_PATH}/include -fprofile-arcs -ftest-coverage

xDayTimeConvM.o: xDayTimeConvM.cpp
	g++ -c xDayTimeConvM.cpp -I${CPPUNIT_PATH}/include -fprofile-arcs -ftest-coverage

clean:
	rm -f *.o
	rm -f *.g*
	rm -f *.*.g*
