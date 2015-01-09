#include "Matrix.hpp"
#include "Vector.hpp"
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

class Matrix_T
{
	public:
	Matrix_T() // Default Constructor, set values that will be used frequently
	{
		eps = 1e-12;

	}
	~Matrix_T() // Default Desructor
	{       // finally delete objects

	} 
	double eps;

	/* Test to check the sizing functions */
	int sizeTest (void)
	{
   		gpstk::Vector<int> v1 = gpstk::Vector<int>(16);
   		for(int i = 0; i < 16; i++) v1[i] = i+1;
   		gpstk::Vector<int> v2 = gpstk::Vector<int>(16);
   		for(int i = 0; i < 16; i++) v2[i] = 16-i;
   		gpstk::Vector<int> v3 = gpstk::Vector<int>(4);
   		for(int i = 0; i < 4; i++) v3[i] = i+1;
   		gpstk::Vector<int> v4 = gpstk::Vector<int>(8);
   		for(int i = 0; i < 4; i++) {v4[i] = i+1+4; v4[i+4] = i+1+4;}

   		gpstk::Matrix<int> a(2,2,1);
   		gpstk::Matrix<int> b(8, 2, 3);
   		gpstk::Matrix<int> c(4, 2, 5);
   		gpstk::Matrix<int> d(4, 4, 7);
   		gpstk::Matrix<int> e(8, 2, v1);
   		gpstk::Matrix<int> f(4, 4, v2);
   		gpstk::Matrix<int> g(2, 2, v3);
   		gpstk::Matrix<int> h(4, 2, v4);

		if ((size_t)4 != a.size()) return 1;
		if ((size_t)2 != a.rows()) return 2;
		if ((size_t)2 != a.cols()) return 3;

		if ((size_t)16 != b.size()) return 4;
		if ((size_t)8 != b.rows()) return 5;
		if ((size_t)2 != b.cols()) return 6;

		if ((size_t)8 != c.size()) return 7;
		if ((size_t)4 != c.rows()) return 8;
		if ((size_t)2 != c.cols()) return 9;

		if ((size_t)16 != d.size()) return 10;
		if ((size_t)4 != d.rows()) return 11;
		if ((size_t)4 != d.cols()) return 12;

		if ((size_t)16 != e.size()) return 13;
		if ((size_t)8 != e.rows()) return 14;
		if ((size_t)2 != e.cols()) return 15;

		if ((size_t)16 != f.size()) return 16;
		if ((size_t)4 != f.rows()) return 17;
		if ((size_t)4 != f.cols()) return 18;

		if ((size_t)4 != g.size()) return 19;
		if ((size_t)2 != g.rows()) return 20;
		if ((size_t)2 != g.cols()) return 21;

		if ((size_t)8 != h.size()) return 22;
		if ((size_t)4 != h.rows()) return 23;
		if ((size_t)2 != h.cols()) return 24;
		return 0;
	}

	/* Test the accessor operator */
	int getTest (void)
	{
   		gpstk::Vector<int> v1 = gpstk::Vector<int>(16);
   		for(int i = 0; i < 16; i++) v1[i] = i+1;
   		gpstk::Vector<int> v2 = gpstk::Vector<int>(16);
   		for(int i = 0; i < 16; i++) v2[i] = 16-i;
   		gpstk::Vector<int> v3 = gpstk::Vector<int>(4);
   		for(int i = 0; i < 4; i++) v3[i] = i+1;
   		gpstk::Vector<int> v4 = gpstk::Vector<int>(8);
   		for(int i = 0; i < 4; i++) {v4[i] = i+1+4; v4[i+4] = i+1+4;}

   		gpstk::Matrix<int> a(2,2,1);
   		gpstk::Matrix<int> b(8, 2, 3);
   		gpstk::Matrix<int> c(4, 2, 5);
   		gpstk::Matrix<int> d(4, 4, 7);
   		gpstk::Matrix<int> e(8, 2, v1);
   		gpstk::Matrix<int> f(4, 4, v2);
   		gpstk::Matrix<int> g(2, 2, v3);
   		gpstk::Matrix<int> h(4, 2, v4);

   		for(int i = 0; i < a.rows(); i++)
      			for(int j = 0; j < a.cols(); j++)
         			if (1 != a(i,j)) return 1;

   		for(int i = 0; i < b.rows(); i++)
      			for(int j = 0; j < b.cols(); j++)
         			if (3 != b(i,j)) return 2;

		for(int i = 0; i < c.rows(); i++)
		      	for(int j = 0; j < c.cols(); j++)
         			if (5 != c(i,j)) return 3;

		for(int i = 0; i < d.rows(); i++)
		      	for(int j = 0; j < d.cols(); j++)
				if (7 != d(i,j)) return 4;

		for(int i = 0; i < e.rows(); i++)
		      	for(int j = 0; j < e.cols(); j++)
				if (1+i+j*8 != e(i,j)) return 5;

		for(int i = 0; i < f.rows(); i++)
		      	for(int j = 0; j < f.cols(); j++)
				if (16-i-4*j != f(i,j)) return 6;
				//cout << f(i,j) << " " << 16-i-4*j << endl;

		for(int i = 0; i < g.rows(); i++)
		      	for(int j = 0; j < g.cols(); j++)
				if (1+i+j*2 != g(i,j)) return 7;
				//cout << g(i,j) << " " << 1+i+j*2 << endl;
		for(int i = 0; i < h.rows(); i++)
		      	for(int j = 0; j < h.cols(); j++)
			 	if (5.+i != h(i,j)) return 8;
				//cout << h(i,j) << " " << 5.+i << endl;
		return 0;
	}

	/* Test the add/subtract and set operators */
	int operatorTest (void)
	{
   		gpstk::Vector<int> v1 = gpstk::Vector<int>(16);
   		for(int i = 0; i < 16; i++) v1[i] = i+1;
   		gpstk::Vector<int> v2 = gpstk::Vector<int>(16);
   		for(int i = 0; i < 16; i++) v2[i] = 16-i;
   		gpstk::Vector<int> v3 = gpstk::Vector<int>(4);
   		for(int i = 0; i < 4; i++) v3[i] = i+1;
   		gpstk::Vector<int> v4 = gpstk::Vector<int>(8);
   		for(int i = 0; i < 4; i++) {v4[i] = i+1+4; v4[i+4] = i+1+4;}

   		gpstk::Matrix<int> a(2,2,1);
   		gpstk::Matrix<int> b(8, 2, 3);
   		gpstk::Matrix<int> c(4, 2, 5);
   		gpstk::Matrix<int> d(4, 4, 7);
   		gpstk::Matrix<int> e(8, 2, v1);
   		gpstk::Matrix<int> f(4, 4, v2);
   		gpstk::Matrix<int> g(2, 2, v3);
   		gpstk::Matrix<int> h(4, 2, v4);

   		a += g; // 2x2 addition
   		for(int i = 0; i < a.rows(); i++)
      			for(int j = 0; j < a.cols(); j++)
				//cout << a(i,j) << " " << 2+i+j*2 << endl;
         			if (2+i+j*2. != a(i,j)) return 1;
   		a -= g; // 2x2 reset a

   		b -= e; // 8x2 subtraction
   			for(int i = 0; i < b.rows(); i++)
      				for(int j = 0; j < b.cols(); j++)
					//cout << b(i,j) << " " << 3-(1+i+j*8) << endl;
         				if (3-(1+i+j*8.) != b(i,j)) return 2;
   		b += e; // 8x2 reset b

   		c -= h; // 4x2 subtraction
   		for(int i = 0; i < c.rows(); i++)
      			for(int j = 0; j < c.cols(); j++)
					//cout << c(i,j) << " " << 5-(5.+i) << endl;
         				if (5-(5.+i) != c(i,j)) return 3;
   		c += h; // 4x2 reset c

   		d += f; // 4x4 addition
   		for(int i = 0; i < d.rows(); i++)
      			for(int j = 0; j < d.cols(); j++)
				//cout << d(i,j) << " " << 16-i-4*j+7 << endl;
         			if (16-i-4*j+7 != d(i,j)) return 4;
   		d -= f; // 4x4 reset d
		return 0;
	}
};

void checkResult(int check, int& errCount) // Function to handle test result output
{
	if (check == -1)
	{
		std::cout << "DIDN'T RUN!!!!" << std::endl;
	}
	else if (check == 0 )
	{
		std::cout << "GOOD!!!!" << std::endl;
	}
	else if (check > 0)
	{
		std::cout << "BAD!!!!" << std::endl;
		std::cout << "Error Message for Bad Test is Code " << check << std::endl;
		errCount++;
	}
}

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	Matrix_T testClass;

	check = testClass.sizeTest();
        std::cout << "sizeTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.getTest();
        std::cout << "getTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.operatorTest();
        std::cout << "opertatorTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	std::cout << "Total Errors: " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
