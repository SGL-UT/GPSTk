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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
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
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
    //----------------------------------------------------------------------
   ObsReader::ObsReader(const string& str, int debug)
      throw(FileMissingException)
      : fn(str), inputType(str), debugLevel(debug), epochCount(0), msid(0),
        usePrevSMOD(false), obsInterval(0.), obsIntervalConfidence(0)
   {
      if (inputType == FFIdentifier::tRinexObs)
      {
         if (debugLevel)
            cout << "Reading " << fn << " as RINEX obs data." << endl;
         ros.open(fn.c_str(), ios::in);
         ros.exceptions(fstream::failbit);
         ros >> roh;
      }
      else if (inputType == FFIdentifier::tMDP)
      {
         if (debugLevel)
            cout << "Reading " << fn << " as MDP data." << endl;
         mdps.open(fn.c_str(), ios::in);
         mdps.exceptions(fstream::failbit);
      }
      else if (inputType == FFIdentifier::tSMODF)
      {
         if (debugLevel)
            cout << "Reading " << fn << " as SMODF data." << endl;
         smos.open(fn.c_str(), ios::in);
         smos.exceptions(fstream::failbit);
      }
      else if (inputType == FFIdentifier::tNovatelOem)
      {
         if (debugLevel)
            cout << "Reading " << fn << " as Novatel OEM data." << endl;
         novas.open(fn.c_str(), ios::in);
         novas.exceptions(fstream::failbit);
      }
      else if (inputType == FFIdentifier::tAshtechSerial)
      {
         if (debugLevel)
            cout << "Reading " << fn << " as Ashtech serial data." << endl;
         ashs.open(fn.c_str(), ios::in);
         ashs.exceptions(fstream::failbit);
      }
   };
#pragma clang diagnostic pop

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

               if (smod.lol!=0)
	       {
                  ObsID oid(ObsID::otLLI, ObsID::cbL1L2, ObsID::tcP);
                  soe[oid] = smod.lol;
	       }
            }
            else if (smod.type==9)
            {
               ObsID oid(ObsID::otPhase, ObsID::cbL1L2, ObsID::tcP);
               soe[oid] = smod.obs;

               if (smod.lol!=0)
	       {
                  ObsID oid(ObsID::otLLI, ObsID::cbL1L2, ObsID::tcP);
                  soe[oid] = smod.lol;
               }
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


   void ObsReader::estimateObsInterval()
   {
      obsIntervalConfidence = 0;
      obsInterval = 0;
      ObsEpoch oe = getObsEpoch();
      CommonTime t0 = oe.time;
      double dt;
   
         // We need 10 consecutive epochs with the same interval to get an
         // acceptable estimate of the obs rate. 
      while (*this && obsIntervalConfidence < 10)
      {
         dt = oe.time - t0;
         if (std::abs(dt - obsInterval) > 0.01)
         {
            obsInterval = dt;
            obsIntervalConfidence = 0;
         }

         ++obsIntervalConfidence;

         t0 = oe.time;
         oe = getObsEpoch();
      }
   }


   //----------------------------------------------------------------------
   ObsReader::operator bool ()
   {
      if (inputType == FFIdentifier::tRinexObs)
         return static_cast<bool>(ros);
      else if (inputType == FFIdentifier::tMDP)
         return static_cast<bool>(mdps);
      else if (inputType == FFIdentifier::tSMODF)
         return static_cast<bool>(smos);
      else if (inputType == FFIdentifier::tNovatelOem)
         return static_cast<bool>(novas);
      else if (inputType == FFIdentifier::tAshtechSerial)
         return static_cast<bool>(ashs);
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
