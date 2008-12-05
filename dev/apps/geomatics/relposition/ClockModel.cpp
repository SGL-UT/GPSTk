#pragma ident "$Id$"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file ClockModel.cpp
 * Implement clock modeling for program DDBase.
 */

//------------------------------------------------------------------------------------
// includes
// system

// GPSTk

// DDBase
#include "DDBase.hpp"
#include "index.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// prototypes -- for this module only
int RemoveClockJumps(void) throw(Exception);             // here
int OutputClockData(void) throw(Exception);              // DataOutput.cpp

//------------------------------------------------------------------------------------
int ClockModel(void) throw(Exception)
{
try {
   int iret=0;

   if(CI.Verbose) oflog << "BEGIN ClockModel()"
      << " at total time " << fixed << setprecision(3)
      << double(clock()-totaltime)/double(CLOCKS_PER_SEC) << " seconds."
      ; //<< endl;
   oflog << " -- ClockModel() is not yet implemented." << endl;

   // output the clock data - Station.ClockBuffer and RxTimeOffset
   // this may be called just before abort in ReadAndProcessRawData, if PRS is far off
   OutputClockData();

   return iret;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end ClockModel()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
