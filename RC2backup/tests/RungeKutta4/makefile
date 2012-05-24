# CppUnit-Tutorial
# file: makefile
# next line has to be changed to the installation-path of CppUnit
CPPUNIT_PATH=/home/ehagen

xRungeKutta4.tst: xRungeKutta4M.o xRungeKutta4.o RungeKutta4.o
	g++ -o xRungeKutta4.tst xRungeKutta4M.o xRungeKutta4.o RungeKutta4.o -L${CPPUNIT_PATH}/lib -lstdc++ -lcppunit -ldl -lgpstk -fprofile-arcs -ftest-coverage


RungeKutta4.o: RungeKutta4.cpp RungeKutta4.hpp
	g++ -c RungeKutta4.cpp -I/home/ehagen/gpstk/src -L/home/ehagen/gpstk/src -fprofile-arcs -ftest-coverage

xRungeKutta4.o: xRungeKutta4.cpp xRungeKutta4.hpp
	g++ -c xRungeKutta4.cpp -I${CPPUNIT_PATH}/include -I/home/ehagen/gpstk/src -L/home/ehagen/gpstk/src -fprofile-arcs -ftest-coverage

xRungeKutta4M.o: xRungeKutta4M.cpp
	g++ -c xRungeKutta4M.cpp -I${CPPUNIT_PATH}/include -fprofile-arcs -ftest-coverage

clean:
	rm -f *.o
	rm -f *.g*
	rm -f *.*.g*