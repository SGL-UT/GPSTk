#pragma ident "$Id$"
/*******************************************************************************/
/* This program reads in clock phase data provided by any of the phase parsers */
/*  and calculates the Overlapping Allan Deviation, outputting the results to  */
/*  the standard output in a format that can be used by allanplot to view the  */
/*  data.                                                                      */
/*                                                                             */
/* Written by Tim Craddock                                                     */
/*******************************************************************************/

#include <iostream>
#include <vector>

#include "AllanDeviation.hpp"

using namespace std;

int main()
{
   // Structures used to store time and clock phase information
   vector <double> timeArray;
   vector <double> phaseArray;
	
   // All of the time and clock phase data is read in from the standard input
   while(!feof(stdin))
   {
      long double time, phase;
      cin >> time >> phase;
      timeArray.push_back(time);
      phaseArray.push_back(phase);
   }
	
   // Ensures there are at least the minimum number of points required to do calculations
   if(timeArray.size() < 2)
   {
      cout << "Not Enough Points to Calculate Tau0" << endl;
      return -1;
   }

   double Tau0 = timeArray[1] - timeArray[0];
   gpstk::AllanDeviation adev(phaseArray, Tau0);
   cout << adev;
   return 0;
}
