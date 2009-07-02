CPPUNIT_PATH=/home/ehagen

xVector.tst: xVectorM.o xVector.o
	g++ -o xVector.tst xVectorM.o xVector.o -L${CPPUNIT_PATH}/lib -lstdc++ -lcppunit -ldl -lgpstk -fprofile-arcs -ftest-coverage

xVector.o: xVector.cpp xVector.hpp
	g++ -c xVector.cpp -I${CPPUNIT_PATH}/include -fprofile-arcs -ftest-coverage

xVectorM.o: xVectorM.cpp
	g++ -c xVectorM.cpp -I${CPPUNIT_PATH}/include -fprofile-arcs -ftest-coverage

clean:
	rm -f *.o
	rm -f *.*.g*
	rm -f *.g*
