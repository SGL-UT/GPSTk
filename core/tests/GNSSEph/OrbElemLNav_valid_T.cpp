//==============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

 /*********************************************************************
*
*  Test program for gpstk/core/lib/GNSSEph/OrbElemRinex.computeBeginValid()
*  and computeEndValid().
*  The last line of output will calculate how many tests fail.
*
*********************************************************************/
#include <iostream>
#include <cmath>
#include <string>

#include "CommonTime.hpp"
#include "CivilTime.hpp"
#include "GNSSconstants.hpp"
#include "SatID.hpp"
#include "TimeString.hpp"

#include "TestUtil.hpp"

#include "OrbElemRinex.hpp"

using namespace gpstk;

int main()
{
   TUDEF( "OrbElemLNav_valid", "" );
   
   /***********************************************/
   //  Test a variety of hand-picked cases to verify that
   //  the beginning of validity time and end of 
   //  validity time are being correctly 
   //  determined.  This bacame more challenging with GPS III
   //  and it is anticiapted that the list of test cases will
   //  expand over time and with experience.
   //
   testFramework.changeSourceMethod( "compute(Begin|End)Valid" );

   int fitHours = 4; 

   SatID gpsSatId = SatID(1,SatID::systemGPS);
   SatID qzsSatId = SatID(MIN_PRN_QZS,SatID::systemQZSS); 

      // Data set cutover without upload cutover.
   CommonTime xmit = CivilTime(2019, 1, 11, 2, 0, 0.0, TimeSystem::GPS); 
   CommonTime toe = CivilTime(2019, 1, 11, 4, 00, 0.0, TimeSystem::GPS);
   CommonTime result = OrbElemRinex::computeBeginValid(gpsSatId,xmit,toe);
   TUASSERTE( CommonTime, xmit, result );

   result = OrbElemRinex::computeBeginValid(qzsSatId,xmit,toe);
   TUASSERTE( CommonTime, xmit, result );

   CommonTime endValid = toe + OrbElemRinex::TWO_HOURS;
   result = OrbElemRinex::computeEndValid(toe, fitHours);
   TUASSERTE( CommonTime, endValid, result );

      // Data set cutover without upload cutover, late receipt
   xmit = CivilTime(2019, 1, 11, 2, 12, 0.0, TimeSystem::GPS); 
   toe = CivilTime(2019, 1, 11, 4, 00, 0.0, TimeSystem::GPS);
   CommonTime expect = CivilTime(2019, 1, 11, 2, 0, 0.0, TimeSystem::GPS); 
   result = OrbElemRinex::computeBeginValid(gpsSatId,xmit,toe);
   TUASSERTE( CommonTime, expect, result );

   result = OrbElemRinex::computeBeginValid(qzsSatId,xmit,toe);
   TUASSERTE( CommonTime, xmit, result );

      // Upload cutover.  Example from prior to GPS III
   xmit = CivilTime(2019, 1, 11, 2, 36, 0.0, TimeSystem::GPS); 
   toe = CivilTime(2019, 1, 11, 3, 59, 44.0, TimeSystem::GPS);
   result = OrbElemRinex::computeBeginValid(gpsSatId,xmit,toe);
   TUASSERTE( CommonTime, xmit, result );

   result = OrbElemRinex::computeBeginValid(qzsSatId,xmit,toe);
   TUASSERTE( CommonTime, xmit, result );

   endValid = toe + OrbElemRinex::TWO_HOURS + OrbElemRinex::SIXTEEN_SECONDS;
   result = OrbElemRinex::computeEndValid(toe, fitHours);
   TUASSERTE( CommonTime, endValid, result );

      // Upload cutover.  Example from early GPS III L1 C/A
   xmit = CivilTime(2019, 1, 9, 22, 30, 00.0, TimeSystem::GPS); 
   toe = CivilTime(2019, 1, 10,  0, 29, 36.0, TimeSystem::GPS);
   result = OrbElemRinex::computeBeginValid(gpsSatId,xmit,toe);
   TUASSERTE( CommonTime, xmit, result );

   endValid = CivilTime(2019, 1, 10,  2, 30, 0.0, TimeSystem::GPS);
   result = OrbElemRinex::computeEndValid(toe, fitHours);
   TUASSERTE( CommonTime, endValid, result );

      // Upload cutover.  Example from GPS III presentation
      // by Steven Brown (L-M) to 2018 Public ICWG
   xmit = CivilTime(2019, 1, 12, 1, 01, 0.0, TimeSystem::GPS); 
   toe = CivilTime(2019, 1, 12, 2, 59, 44.0, TimeSystem::GPS);
   result = OrbElemRinex::computeBeginValid(gpsSatId,xmit,toe);
   TUASSERTE( CommonTime, xmit, result );

   endValid = CivilTime(2019, 1,12, 5, 0, 0.0, TimeSystem::GPS);
   result = OrbElemRinex::computeEndValid(toe, fitHours);
   TUASSERTE( CommonTime, endValid, result );

   xmit = CivilTime(2019, 1, 12, 1, 16, 0.0, TimeSystem::GPS); 
   result = OrbElemRinex::computeBeginValid(gpsSatId,xmit,toe);
   TUASSERTE( CommonTime, xmit, result );

   result = OrbElemRinex::computeEndValid(toe, fitHours);
   TUASSERTE( CommonTime, endValid, result );

      //  2
   xmit = CivilTime(2019, 1, 12, 1, 31, 0.0, TimeSystem::GPS); 
   result = OrbElemRinex::computeBeginValid(gpsSatId,xmit,toe);
   TUASSERTE( CommonTime, xmit, result );

   result = OrbElemRinex::computeEndValid(toe, fitHours);
   TUASSERTE( CommonTime, endValid, result );

      //  3
   xmit = CivilTime(2019, 1, 12, 1, 46, 0.0, TimeSystem::GPS); 
   result = OrbElemRinex::computeBeginValid(gpsSatId,xmit,toe);
   TUASSERTE( CommonTime, xmit, result );

   result = OrbElemRinex::computeEndValid(toe, fitHours);
   TUASSERTE( CommonTime, endValid, result );

   std::cerr << "Ending program." << std::endl;
   std::cout << "Total Failures for " << __FILE__ << ": "
             << testFramework.countFails() << std::endl;
             
   return testFramework.countFails();
             
}
   
