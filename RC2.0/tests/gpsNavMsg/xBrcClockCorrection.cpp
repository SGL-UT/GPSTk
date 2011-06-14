/*********************************************************************
*  $Id:$
*
*  Test program from June 2011.  Written to test the BrcKeplerOrbit 
*  module as it was being developed.
*
*********************************************************************/
#include <stdio.h>

#include "BrcClockCorrection.hpp"
#include "EngEphemeris.hpp"

using namespace std;
using namespace gpstk;

int main( int argc, char * argv[] )
{
      // Set time to Day 153, 2011 (6/2/2011) at noon
   DayTime dt( 2011, 6, 2, 12, 14, 44.0 );

      // Test data (copied from navdmp output for .....)
      // Generally, we'd load these data from the file
   char SysID = 'G';
   ObsID obsID( ObsID::otUndefined, ObsID::cbL1, ObsID::tcCA );
   short PRNID = 3;
   double Toc =              388800.0;
   short weeknum =               1638;     // By rules of Kepler Orbit, this must be week of Toc
   double accuracy =            10.61;
   bool healthy =                true;
   double af0 =        7.23189674E-04;
   double af1 =        5.11590770E-12;
   double af2 =        0.00000000E+00;
  
      //Test Data copied from RINEX file	
   double rToc =              388800.0;
   short rweeknum =               1638;     // By rules of Kepler Orbit, this must be week of Toc
   double raccuracy =            10.61;
   bool rhealthy =                true;
   double raf0 =        7.23189674318E-04;
   double raf1 =        5.11590769747E-12;
   double raf2 =        0.00000000000E+00;

   long subframe1[10] = { 0x22C2663D, 0x1F0E29B8, 0x2664002B, 0x09FCC1B6, 0x0F60EB8A,
                          0x1299CE93, 0x29CD3DB6, 0x0597BB0F, 0x00000B68, 0x17B28E5C };
   long subframe2[10] = { 0x22C2663D, 0x1F0E4A28, 0x05809675, 0x0EBD8AF1, 0x00089344,
                          0x008081F8, 0x1330CC2C, 0x0461E855, 0x034F8045, 0x17BB1E68 };
   long subframe3[10] = { 0x22C2663D, 0x1F0E6BA0, 0x3FE129CD, 0x26E31837, 0x0006C96A,
                          0x35A74DFC, 0x065C8B0F, 0x1E4F400A, 0x3FE8966D, 0x05860C44 };


      // First test case.  Create an empty CC object, then load the data.
   cout << "Test Case 1: Creating an empty CC object and loading the data." << endl;
   BrcClockCorrection co1;
   co1.loadData( SysID, obsID, PRNID, Toc, weeknum, accuracy, healthy, 
		 af0, af1, af2 ); 

   double ClkCorr1 = co1.svClockBias( dt );
   cout.precision(11); 
   cout << "Clock Correction co1: " << ClkCorr1 << endl;

      // Second test case.  Create an CC object with data available at time of construction.
   cout << "Test Case 2: Creating CC object with data." << endl;
   BrcClockCorrection co2( SysID, obsID, PRNID, Toc, weeknum, accuracy, healthy, 
		 af0, af1, af2 ); 

   double ClkCorr2 = co2.svClockBias( dt ); 
   cout << "ClockCorrection co2: " << ClkCorr2 << endl;

      // Third test case.  Create a CC object using raw legacy navigation message data
   cout << "Test Case 3: Creating CC object with raw legacy nav message data." << endl;
   BrcClockCorrection co3(obsID, PRNID, weeknum, subframe1 );

   double ClkCorr3 = co3.svClockBias( dt ); 
   cout << "Clock Correction co3: " << ClkCorr3 << endl;

      // Fourth test case.  Create a CC object using raw legacy navigation message data
   cout << "Test Case 4: Creating CC object with raw legacy nav message data." << endl;
   BrcClockCorrection co4;
   co4.loadData(obsID, PRNID, weeknum, subframe1 );

   double ClkCorr4 = co4.svClockBias( dt ); 
   cout << "Clock Correction co4: " << ClkCorr4 << endl;


      // Fifth test case.  Create an CC object with data available from RINEX file.
   cout << "Test Case 5: Creating CC object with data from RINEX file." << endl;
   BrcClockCorrection co5( SysID, obsID, PRNID, rToc, rweeknum, raccuracy, rhealthy, 
		 raf0, raf1, raf2 ); 

   double ClkCorr5 = co5.svClockBias( dt ); 
   cout << "Clock Correction co5: " << ClkCorr5 << endl;

      // Sixth test case.  Compare against "classic" EngEphemeris
   cout << "Test Case 6: Calculated clock correction using 'classic' EngEphemeris." << endl;
   EngEphemeris EE;
   EE.addSubframe(subframe1, weeknum, 3, 1);
   EE.addSubframe(subframe2, weeknum, 3, 1);
   EE.addSubframe(subframe3, weeknum, 3, 1);

   Xvt xvt = EE.svXvt(dt);
   cout<< "Clock Correction EE: " << xvt.dtime <<endl;
   //cout<< " EE: " << xvt.ddtime <<endl;
   
   return(0);
}
