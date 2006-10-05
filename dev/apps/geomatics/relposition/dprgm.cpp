#include <iostream>
#include <string>
#include "Position.hpp"
#include "Matrix.hpp"
#include "Namelist.hpp"
using namespace std;
using namespace gpstk;
int main(void)
{
   try {
      Namelist NL;
      NL += string("test");
      NL += string("Fred");
      NL += string("Millie");
      NL += string("Constipation");
      NL += string("Seredipity");
      cout << "Namelist : " << NL << endl;
      Position DREF(-1490091.331,-5147246.750,3449878.007);
      Position B150(-1491081.525,-5147646.073,3448838.443);
      Position B151(-1491078.993,-5147629.677,3448863.890);
      Position B152(-1491076.460,-5147613.270,3448889.334);
      cout << "DREF " << DREF << endl;
      cout << "B150 " << B150 << endl;
      cout << "B151 " << B151 << endl;
      cout << "B152 " << B152 << endl;
      cout << "BL DREF-B150 " << (DREF-B150) << " " << range(DREF,B150) << endl;
      cout << "BL DREF-B151 " << (DREF-B151) << " " << range(DREF,B151) << endl;
      cout << "BL DREF-B152 " << (DREF-B152) << " " << range(DREF,B152) << endl;
      cout << "BL B150-B151 " << (B150-B151) << " " << range(B150,B151) << endl;
      cout << "BL B150-B152 " << (B150-B152) << " " << range(B150,B152) << endl;
      cout << "BL B151-B152 " << (B151-B152) << " " << range(B151,B152) << endl;
   }
   catch(Exception& e) {
      cerr << "Caught exception " << e << endl;
      return -1;
   }

   Matrix<double> A(2,2);
   A(0,0) = .3775;
   A(0,1) = .107;
   A(1,0) = .244;
   A(1,1) = -.054;

   cout << "Matrix A(" << A.rows() << "," << A.cols() << "):\n"
      << fixed << setprecision(2) << setw(5) << A << endl;
   cout << "Determinant of A = " << scientific << setprecision(3) << slowDet(A) << endl;

   Matrix<double> Ainv;
   try { Ainv=inverse(A); }
   catch (Exception& e) { cout << e << endl; return -1;}
   cout << "Inverse matrix (" << Ainv.rows() << "," << Ainv.cols() << ") :\n"
      << fixed << setprecision(4) << setw(7) << Ainv << endl;

   return 0;
}
