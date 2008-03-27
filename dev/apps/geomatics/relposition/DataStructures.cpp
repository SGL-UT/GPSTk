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
 * @file DataStructures.cpp
 * Implement various data containers for program DDBase.
 */

//------------------------------------------------------------------------------------
// system includes

// GPSTk

// DDBase
#include "DDBase.hpp"
#include "DataStructures.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
Station& findStationInList(map<string,Station>& SL, string& label)
{
try {
   map<string,Station>::const_iterator it;
   it = SL.find(label);
   if(it == SL.end()) {                      // create a new Station
      Station st;
      SL[label] = st;
   }
   return SL[label];
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
Station::Station(void) throw()
{
   fixed = usePRS = false;
   temp = press = rhumid = 0.0;
   time = DayTime::BEGINNING_OF_TIME;
   pTropModel = NULL;
   TropType = "Saas";
   ant_azimuth = 0.0;
}

//------------------------------------------------------------------------------------
Station::~Station(void) throw()
{
   delete pTropModel;
}

//------------------------------------------------------------------------------------
ObsFile::ObsFile(void) throw()
{
   name = string("");
   label = string("");
   nread = -1;
   valid = false;
   inC1 = -1;
   inP1 = -1; inP2 = -1;
   inL1 = -1; inL2 = -1;
   inD1 = -1; inD2 = -1;
   inS1 = -1; inS2 = -1;
}

//------------------------------------------------------------------------------------
ObsFile& ObsFile::operator=(const ObsFile& of) throw()
{
   name = of.name;
   label = of.label;
   nread = of.nread;     // number of records read (-1=unopened, 0=header read)
   valid = of.valid;
   inC1 = of.inC1;
   inP1 = of.inP1; inP2 = of.inP2;
   inL1 = of.inL1; inL2 = of.inL2;
   inD1 = of.inD1; inD2 = of.inD2;
   inS1 = of.inS1; inS2 = of.inS2;
   return *this;
}

//------------------------------------------------------------------------------------
ObsFile::ObsFile(const ObsFile& of) throw()
{
   *this = of;
}

//------------------------------------------------------------------------------------
ObsFile::~ObsFile(void)
{
   ins.clear();
   ins.close();
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
