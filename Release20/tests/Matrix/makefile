CPPUNIT_PATH=/home/ehagen

xMatrix.tst: xMatrixM.o xMatrix.o
	g++ -o xMatrix.tst xMatrixM.o xMatrix.o -L${CPPUNIT_PATH}/lib -lstdc++ -lcppunit -ldl -lgpstk -fprofile-arcs -ftest-coverage

xMatrix.o: xVMatrix.cpp xVector.hpp
	g++ -c xMatrix.cpp -I${CPPUNIT_PATH}/include -fprofile-arcs -ftest-coverage

xMatrixM.o: xMatrixM.cpp
	g++ -c xMatrixM.cpp -I${CPPUNIT_PATH}/include -fprofile-arcs -ftest-coverage

clean:
	rm -f *.o
	rm -f *.*.g*
	rm -f *.g*
