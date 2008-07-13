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


/** @file Read various file formats and output a stream of ObsEpoch objects.
*/

#include "ObsUtils.hpp"

#include "RinexObsData.hpp"
#include "SMODFData.hpp"
#include "MDPObsEpoch.hpp"
#include "ObsReader.hpp"
#include "NovatelData.hpp"

using namespace std;

namespace gpstk
{
   //----------------------------------------------------------------------
   ObsReader::ObsReader(const string& str, int verbose)
      throw(FileMissingException)
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
         mdps.exceptions(fstream::failbit);
      }
      else if (inputType == FFIdentifier::tSMODF)
      {
         if (verboseLevel)
            cout << "# Reading " << fn << " as SMODF data." << endl;
         smos.open(fn.c_str(), ios::in);
         smos.exceptions(fstream::failbit);
      }
      else if (inputType == FFIdentifier::tNovatelOem)
      {
         if (verboseLevel)
            cout << "# Reading " << fn << " as Novatel OEM data." << endl;
         novas.open(fn.c_str(), ios::in);
         novas.exceptions(fstream::failbit);
      }
      else if (inputType == FFIdentifier::tAshtechSerial)
      {
         if (verboseLevel)
            cout << "# Reading " << fn << " as Ashtech serial data." << endl;
         ashs.open(fn.c_str(), ios::in);
         ashs.exceptions(fstream::failbit);
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
         while (smos)
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
      else if (inputType == FFIdentifier::tNovatelOem)
      {
         NovatelData nd;
         novas >> nd;
         cout << "Not implimented yet." << endl;
         epochCount++;
      }
      else if (inputType == FFIdentifier::tAshtechSerial)
      {
         AshtechData d;
         ashs >> d;
         cout << "Ocibu bites" << endl;
         epochCount++;
      }
      return oe;
   }


   double ObsReader::estimateObsInterval()
   {
      int j=0;
      double epochRate = 0;
      ObsEpoch oe = getObsEpoch();
      DayTime t0 = oe.time;
   
      // We need 10 consecutive epochs with the same interval that occur
      // during the first 100 epochs of the input to get an acceptable estimate
      // of the obs rate
      for (int i=0; i<100 && !(*this); i++)
      {
         double dt = oe.time - t0;
         if (std::abs(dt - epochRate) > 0.01)
         {
            epochRate = dt;
            j = 0;
         }
         else
            j++;

         if (j >10)
            return epochRate;

         t0 = oe.time;
         *this >> oe;
      }
      
      return -1;
   }


   //----------------------------------------------------------------------
   ObsReader::operator bool ()
   {
      if (inputType == FFIdentifier::tRinexObs)
         return ros;
      else if (inputType == FFIdentifier::tMDP)
         return mdps;
      else if (inputType == FFIdentifier::tSMODF)
         return smos;
      else if (inputType == FFIdentifier::tNovatelOem)
         return novas;
      else if (inputType == FFIdentifier::tAshtechSerial)
         return ashs;
      return false;
   }


   //----------------------------------------------------------------------
   ObsReader& operator>>(ObsReader& obsReader, ObsEpoch& f)
      throw()
   {
      f = obsReader.getObsEpoch();
      return obsReader;
   }

} // end of namespace gpstk
