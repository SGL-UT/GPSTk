
#include <iostream>

#include "Matrix.hpp"
#include "Vector.hpp"

using namespace std;

void SVDTest(size_t r, size_t c,
             double xA[], double xB[], double xBSref[])
{
   gpstk::Matrix<double> A(r,c);
   A = xA;
   gpstk::SVD<double> svd;
   svd(A);
   gpstk::Matrix<double> S(r, c, 0.0);
   for (int i=0; i<r; i++)
      S(i,i) = svd.S(i);

   cout << " A " << r << "x" << c << ":" << endl
        << A << endl;
   cout << "A - svd.U * S * transpose(svd.V):" << endl
        << A - svd.U * S * transpose(svd.V) << endl;
   cout << " I - svd.U * transpose(svd.U):" << endl
        << gpstk::ident<double>(r) - svd.U * transpose(svd.U) << endl;
   cout << " I - svd.V * transpose(svd.V):" << endl
        << gpstk::ident<double>(c) - svd.V * transpose(svd.V) << endl;

   if (r == c)
   {
      gpstk::Vector<double> B(r), BSref(r);
      B = xB;
      BSref = xBSref;
      svd.backSub(B);
      cout << "B - BSref:" << endl
           << B - BSref << endl;
   }
   cout << "----------" << endl;
}


int main()
{
   double a44[] = {2,-1,0,0,-1,2,-1,0,0,-1,2,-1,0,0,-1,2};
   double b4[] = {5,1,-2,6};
   double bs4[] ={5,5,4,5};   
   
   double a22[] = {2,1,1,2};
   double b2[] = {1,2};
   double bs2[] = {0,1};
   SVDTest(2, 2, a22, b2, bs2);

   double a23[] = {4, 11, 14, 8, 7, -2};
   SVDTest(2, 3, a23, NULL, NULL);
   SVDTest(3, 2, a23, NULL, NULL);

   double a33[] = {2,-1,0,-1,2,-1,0,-1,2};
   double b3[] = {7,-3,2};
   double bs3[] = {4.25,1.5,1.75};
   SVDTest(3, 3, a33, b3, bs3);

   SVDTest(4, 4, a44, b4, bs4);
   SVDTest(2, 8, a44, NULL, NULL);
   SVDTest(8, 2, a44, NULL, NULL);
   cout << "barf" << endl;
   gpstk::SVD<double> foo;
   cout << "bar" << endl;
}
