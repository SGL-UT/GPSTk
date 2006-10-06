#pragma ident "$Id: DataAvailabilityAnalyzer.cpp 179 2006-10-05 14:22:48Z ocibu $"

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


/** @file Read various file formats and output a stream of ObsEpoch objects.
*/

#include "ObsUtils.hpp"

#include "RinexObsData.hpp"
#include "SMODFData.hpp"
#include "MDPObsEpoch.hpp"
#include "ObsReader.hpp"

using namespace std;

namespace gpstk
{
   ObsReader::ObsReader(const string& str)
      : fn(str), inputType(str), verboseLevel(0), epochCount(0), msid(0)
   {
      if (inputType == FFIdentifier::tRinexObs)
      {
         if (verboseLevel)
            cout << "Reading " << fn << " as RINEX obs data." << endl;
         ros.open(fn.c_str(), ios::in);
         ros.exceptions(fstream::failbit);
         ros >> roh;
      }
      else if (inputType == FFIdentifier::tMDP)
      {
         if (verboseLevel)
            cout << "Reading " << fn << " as MDP data." << endl;
         mdps.open(fn.c_str(), ios::in);
      }
      else if (inputType == FFIdentifier::tSMODF)
      {
         if (verboseLevel)
            cout << "Reading " << fn << " as SMODF data." << endl;
         smos.open(fn.c_str(), ios::in);
         smos.exceptions(fstream::failbit);
      }
   };

   ObsEpoch ObsReader::getObsEpoch()
   {
      ObsEpoch oe;
      if (inputType == FFIdentifier::tRinexObs)
      {
         RinexObsData rod;
         ros >> rod;
         oe = makeObsEpoch(rod);
      }
      else if (inputType == FFIdentifier::tMDP)
      {
         MDPEpoch moe;
         mdps >> moe;
         oe = makeObsEpoch(moe);
      }
      else if (inputType == FFIdentifier::tSMODF)
      {
         SMODFData smod;
         smos >> smod;
         cout << "write me" << endl;
      }

      epochCount++;
      return oe;
   }

   bool ObsReader::operator()()
   {
      if (inputType == FFIdentifier::tRinexObs)
         return ros;
      else if (inputType == FFIdentifier::tMDP)
         return mdps;
      else if (inputType == FFIdentifier::tSMODF)
         return smos;
      return false;
   }
} // end of namespace gpstk

/*
   // ---------------------------------------------------------------------
   // ---------------------------------------------------------------------
   void DataReader::read_smo_data(const string& fn)
   {
      SMODFStream smo(fn.c_str(), ios::in);
      smo.exceptions(fstream::failbit);
      if (verbosity>2)
         cout << "Reading " << fn << " as SMODF."<< endl;

      SMODFData smodata;

      while (smo >> smodata)
      {
         if (smodata.station != msid)
            continue;

         const DayTime& t = smodata.time;
         if (t<startTime || t>stopTime)
            continue;

         ObsEpoch& oe = oem[t];
         oe.time = t;

         SatID svid(smodata.PRNID, SatID::systemGPS);
         SvObsEpoch& soe = oe[svid];
         soe.svid=svid;

         soe[getObsID(smodata)] = smodata.obs * 1000;
      }

      haveObsData = true;
      if (verbosity>1)
         cout << "Read " << fn << " as SMODF obs."<< endl;

   } // end of read_smo_data()
*/
