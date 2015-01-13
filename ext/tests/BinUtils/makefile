# file: makefile

CPPUNIT_PATH=/home/ehagen

xBinUtils.tst: xBinUtilsM.o xBinUtils.o BinUtils.o
	g++ -o xBinUtils.tst xBinUtilsM.o xBinUtils.o BinUtils.o -L${CPPUNIT_PATH}/lib -lstdc++ -lcppunit -ldl -lgpstk -fprofile-arcs -ftest-coverage

BinUtils.o: /home/ehagen/gpstk/src/BinUtils.cpp /home/ehagen/gpstk/src/BinUtils.hpp
	g++ -c /home/ehagen/gpstk/src/BinUtils.cpp -I/home/ehagen/gpstk/src -L/home/ehagen/gpstk/src -fprofile-arcs -ftest-coverage

xBinUtils.o: xBinUtils.cpp xBinUtils.hpp
	g++ -c xBinUtils.cpp -I${CPPUNIT_PATH}/include -I/home/ehagen/gpstk/src -L/home/ehagen/gpstk/src -fprofile-arcs -ftest-coverage

xBinUtilsM.o: xBinUtilsM.cpp
	g++ -c xBinUtilsM.cpp -I${CPPUNIT_PATH}/include -fprofile-arcs -ftest-coverage

clean:
	rm -f *.o
	rm -f *.g*
	rm -f *.*.g*
