#include <iostream>
#include <iomanip>

#include "RinexObsStream.hpp"
#include "Expression.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{
   short test=1;
   
   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {  
      string istr(" 1   + (6 - 2 ) * 3.2");
      Expression xpr(istr);
      cout << "Input string: \"" << istr << "\"" << endl;
      xpr.print(cout);
      cout << "=" << xpr.evaluate() << endl;
   }

   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
      try {  
         string istr(" 1   + (6 - gamma ) * 3.2");
         Expression xpr(istr);
         cout << "Input string: \"" << istr << "\"" << endl;
         xpr.print(cout);
         cout << "=" << xpr.evaluate() << endl;
      }
      catch (gpstk::Expression::ExpressionException ee)
      {
         cout << endl << "The expected exception was generated: " << endl;
         cout << ee << endl;
      }
   
   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      string istr(" 1 + 2*(3 + 1)"); 
      Expression xpr(istr);
      cout << "Input string: \"" << istr << "\"" << endl;
      xpr.print(cout);
      cout << "=" << xpr.evaluate() << endl;
   }

   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      try {
         string istr(" 1 + 2*( beta + 1)"); 
         Expression xpr(istr);
         cout << "Input string: \"" << istr << "\"" << endl;
         xpr.print(cout);
         xpr.set("beta",1);
         cout << "=" << xpr.evaluate() << endl;
      }
      catch (...)
      {
         cout << "An unexpected exception was generated: ";
      }
   }

   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      string istr(" 1 + 2*cos(3.141592647)"); 
      Expression xpr(istr);
      cout << "Input string: \"" << istr << "\"" << endl;
      xpr.print(cout);
      cout << "=" << xpr.evaluate() << endl;
   }

   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      string istr(" 1E+1 + 4* 2E-2"); 
      Expression xpr(istr);
      cout << "Input string: \"" << istr << "\"" << endl;
      xpr.print(cout);
      cout << "=" << xpr.evaluate() << endl;
   }

   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      string istr("C/L1"); 
      Expression xpr(istr);
      xpr.setGPSConstants();
      cout << "Input string: \"" << istr << "\"" << endl;
      xpr.print(cout);
      cout << "=" << xpr.evaluate() << endl;
   }

   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      string istr("1/(1-gamma)*(P1 - P2)"); 
      cout << "Input string: \"" << istr << "\"" << endl;
      Expression xpr(istr);
      xpr.print(cout);
      cout << endl;
      xpr.setGPSConstants();
      
      RinexObsStream ros("../examples/bahr1620.04o");
      RinexObsData rod;
      for (int i=0;i<3;i++)
         //while (ros >> rod)
      {
         ros >> rod;

         RinexObsData::RinexPrnMap::const_iterator it;
         for (it = rod.obs.begin(); it!= rod.obs.end(); it++)
         {
            xpr.setRinexObs(it->second);
            cout << rod.time << " " << it->first.prn << " ";
            cout << xpr.evaluate() << endl;
         } // end step through PRNs in an epoch
      } // end step through input obs file epoch by epoch
   }
   
   exit(0);   
}
