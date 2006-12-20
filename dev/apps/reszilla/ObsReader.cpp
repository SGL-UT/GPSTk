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
   //----------------------------------------------------------------------
   ObsReader::ObsReader(const string& str, int verbose)
      : fn(str), inputType(str), verboseLevel(verbose), epochCount(0), msid(0),
        usePrevSMOD(false)
   {
      if (inputType == FFIdentifier::tRinexObs)
      {
         if (verboseLevel)
            cout << "# Reading " << fn << " as RINEX obs data." << endl;
         ros.open(fn.c_str(), ios::in);
         ros.exceptions(fstream::failbit);
         ros >> roh;
      }
      else if (inputType == FFIdentifier::tMDP)
      {
         if (verboseLevel)
            cout << "# Reading " << fn << " as MDP data." << endl;
         mdps.open(fn.c_str(), ios::in);
      }
      else if (inputType == FFIdentifier::tSMODF)
      {
         if (verboseLevel)
            cout << "# Reading " << fn << " as SMODF data." << endl;
         smos.open(fn.c_str(), ios::in);
         smos.exceptions(fstream::failbit);
      }
   };


   //----------------------------------------------------------------------
   ObsEpoch ObsReader::getObsEpoch()
   {
      ObsEpoch oe;
      if (inputType == FFIdentifier::tRinexObs)
      {
         RinexObsData rod;
         ros >> rod;
         oe = makeObsEpoch(rod);
         epochCount++;
      }
      else if (inputType == FFIdentifier::tMDP)
      {
         MDPEpoch moe;
         mdps >> moe;
         oe = makeObsEpoch(moe);
         epochCount++;
      }
      else if (inputType == FFIdentifier::tSMODF)
      {
         if (msid==0)
         {
            cerr << "SMODF data requires an msid to be specified. Exiting." << endl;
            exit(-1);
         }
         
         SMODFData smod;
         while (true)
         {
            if (usePrevSMOD)
            {
               smod = prevSMOD;
               usePrevSMOD = false;
            }
            else
            {
               while (smos >> smod)
                  if (smod.station == msid) break;
            }

            if (smod.station != msid)
               break;

            if (smod.time != oe.time && oe.size())
            {
               usePrevSMOD = true;
               prevSMOD = smod;
               epochCount++;
               break;
            }

            oe.time = smod.time;
            
            SatID svid(smod.PRNID, SatID::systemGPS);
            SvObsEpoch& soe = oe[svid];
            soe.svid=svid;
            
            if (smod.type==0)
            {
               ObsID oid(ObsID::otRange, ObsID::cbL1L2, ObsID::tcP);
               soe[oid] = smod.obs * 1000;
            }
            else if (smod.type==9)
            {
               ObsID oid(ObsID::otPhase, ObsID::cbL1L2, ObsID::tcP);
               soe[oid] = smod.obs;
            }
         }
      }

      return oe;
   }


   //----------------------------------------------------------------------
   bool ObsReader::operator()()
   {
      if (inputType == FFIdentifier::tRinexObs)
         return true;
      else if (inputType == FFIdentifier::tMDP)
         return true;
      else if (inputType == FFIdentifier::tSMODF)
         return true;
      return false;
   }
} // end of namespace gpstk
