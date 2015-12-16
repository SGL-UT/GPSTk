
#include <iostream>

#include "Matrix.hpp"
#include "Vector.hpp"

using namespace std;

int debug = 0;

void SVDTest(size_t rows, size_t cols,
             double xA[], double xB[], double xBSref[])
{
   gpstk::Matrix<double> A(rows,cols);
   A = xA;
   gpstk::SVD<double> svd;
   svd(A);
   gpstk::Matrix<double> S(rows, cols, 0.0);
   for (int i=0; i<min(rows,cols); i++)
      S(i,i) = svd.S(i);

   cout << "A " << rows << "x" << cols << ":" << endl;
   
   gpstk::Matrix<double> USV,UUT,VVT,Ir, Ic;
   USV = svd.U * S * transpose(svd.V);
   UUT = svd.U * transpose(svd.U);
   VVT = svd.V * transpose(svd.V);
   Ir = gpstk::ident<double>(rows);
   Ic = gpstk::ident<double>(cols);
   
   cout << "maxabs(A-USV)=" << gpstk::maxabs(A-USV)
        << ", maxabs(I-UUT)=" << gpstk::maxabs(Ir - UUT)
        << ", maxabs(I-VVT)=" << gpstk::maxabs(Ic - VVT) << endl;
   if (debug)
      cout << "A" << endl << A << endl
           << "A-USV" << endl << A-USV << endl
           << "I-UUT" << endl << Ir-UUT << endl
           << "I-VVT" << endl << Ic-VVT << endl;

   if (rows == cols)
   {
      gpstk::Vector<double> B(rows), BSref(rows);
      B = xB;
      BSref = xBSref;
      svd.backSub(B);
      if (debug)
         cout << "B - BSref:" << endl
              << B - BSref << endl;
   }
   cout << endl;
}


int main()
{
   double a22[] = {2,1,1,2};
   double b2[] = {1,2};
   double bs2[] = {0,1};

   double a23[] = {4, 11, 14, 8, 7, -2};

   double a33[] = {2,-1,0,-1,2,-1,0,-1,2};
   double b3[] = {7,-3,2};
   double bs3[] = {4.25,1.5,1.75};

   double a44[] = {2,-1,0,0,-1,2,-1,0,0,-1,2,-1,0,0,-1,2};
   double b4[] = {5,1,-2,6};
   double bs4[] ={5,5,4,5};   

   SVDTest(4, 4, a44, b4, bs4);
   SVDTest(8, 2, a44, NULL, NULL);
   SVDTest(2, 8, a44, NULL, NULL);

   SVDTest(2, 2, a22, b2, bs2);

   SVDTest(2, 3, a23, NULL, NULL);
   SVDTest(3, 2, a23, NULL, NULL);

   SVDTest(3, 3, a33, b3, bs3);

}
