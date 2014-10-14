CPPUNIT_PATH=/home/ehagen

xStats.tst: xStatsM.o xStats.o Stats.o
	g++ -o xStats.tst xStatsM.o xStats.o Stats.o -L${CPPUNIT_PATH}/lib -lstdc++ -lcppunit -ldl -lgpstk -fprofile-arcs -ftest-coverage

Stats.o: Stats.cpp
	g++ -c Stats.cpp -I/home/ehagen/gpstk/src -L/home/ehagen/gpstk/src -lgpstk -fprofile-arcs -ftest-coverage

xStats.o: xStats.cpp xStats.hpp
	g++ -c xStats.cpp -I${CPPUNIT_PATH}/include -fprofile-arcs -ftest-coverage

xStatsM.o: xStatsM.cpp
	g++ -c xStatsM.cpp -I${CPPUNIT_PATH}/include -fprofile-arcs -ftest-coverage

clean:
	rm -f *.o
	rm -f *.*.g*
	rm -f *.g*
