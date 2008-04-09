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

/*
   Computes several dilution of precision (DOP) parameters:
      PDOP - Position dilution of precision
      TDOP - Time dilution of precision
      GDOP - Geometric dilution of precision
*/

#include <iostream>

#include "BasicFramework.hpp"
#include "CommandOptionWithTimeArg.hpp"
#include "EphReader.hpp"
#include "ObsReader.hpp"
#include "MSCData.hpp"
#include "MSCStream.hpp"
#include "TropModel.hpp"
#include "PRSolution.hpp"
#include "DOP.hpp"

using namespace std;
using namespace gpstk;

class DOPCalc : public BasicFramework
{
public:
   DOPCalc(const string& applName) throw()
   : BasicFramework(
         applName,
         "Computes PDOP, TDOP, and GDOP."),
   rxPosOpt(
      'p',
      "position",
      "User position in ECEF (x,y,z) coordinates.  Format as a string: "
      "\"X Y Z\"."),
   minElevOpt(
      '\0',
      "el-mask",
      "Elevation mask to apply, in degrees. The default is 0."),
   ephFileOpt(
      'e',
      "eph",
      "Where to get the ephemeris data. Acceptable formats include " 
       + EphReader::formatsUnderstood() + ". Repeat for multiple files.",
       true),
   obsFileOpt(
      'o',
      "obs",
      "Where to get the observation data. Acceptable formats include "
      + ObsReader::formatsUnderstood() + ". Repeat for multiple files. "
      "If a RINEX obs file is provided, the position will be taken from the "
      "header unless otherwise specified.",
      true),
   mscFileOpt(
      'c',
      "msc",
      "Station coordinate file."),
   msidOpt(
      'm',
      "msid",
      "Monitor station ID number."),
   
   minElev(0), msid(0)
   
   {};

   bool initialize(int argc, char *argv[]) throw();   

protected:
   virtual void spinUp();

   virtual void process();
   
   virtual void shutDown() {};
   
private:
   CommandOptionWithAnyArg rxPosOpt, minElevOpt, ephFileOpt, obsFileOpt, 
                           mscFileOpt, msidOpt;
   EphReader ephReader;
   double minElev;
   DayTime startTime, stopTime;
   Triple rxPos;
   Xvt rxXvt;
   unsigned msid;
   ObsEpochMap obsEpochMap;
   
   void readObsFile(const CommandOptionWithAnyArg& obsFileOpt, 
                    ObsEpochMap &oem);
};  

bool DOPCalc::initialize(int argc, char *argv[]) throw()
{
   if (!BasicFramework::initialize(argc,argv)) return false;

      // if specified, get the elevation mask
   if (minElevOpt.getCount())
      minElev = StringUtils::asDouble((minElevOpt.getValue())[0]);
           
      // set verbose level
   ephReader.verboseLevel = verboseLevel;
   
      // set debug level
   FFIdentifier::debugLevel = debugLevel;
   
      // read in ephemeris data
   for (int i=0; i<ephFileOpt.getCount(); i++)
      ephReader.read(ephFileOpt.getValue()[i]);

      // grab the station number (needed to process smooth data)
   if (msidOpt.getCount())
      msid = StringUtils::asUnsigned(msidOpt.getValue()[0]);
      
      // read in observation data
   readObsFile(obsFileOpt, obsEpochMap);
    
      // get the antenna position (if RINEX was given, this will overwrite pos.)
   if (rxPosOpt.getCount())
   {
         // get the position from the command line option
      double x,y,z;
      sscanf(rxPosOpt.getValue().front().c_str(),"%lf %lf %lf", &x, &y, &z);
      rxPos[0] = x;
      rxPos[1] = y;
      rxPos[2] = z;      
   }
   else if (msidOpt.getCount() && mscFileOpt.getCount())
   {
         // get the position from the MSC file
      string fn = mscFileOpt.getValue()[0];
      MSCStream mscs(fn.c_str(), ios::in);
      MSCData mscd;
      while (mscs >> mscd)
      {
         if (mscd.station == msid)
         {
            rxPos = mscd.coordinates;
            
            if (debugLevel || verboseLevel)
               cout << "Read position from MSC file: " 
                    << fn << endl;
            break;
         }
      }     
   }
  
      // create the receiver Xvt
   rxXvt.x = rxPos;
   
   return true;
}

void DOPCalc::spinUp()
{
      // check on ephemeris data
   if (ephReader.eph == NULL)
   {
      cout << "Didn't get any ephemeris data from the eph files. "
           << "Exiting." << endl;
      exit(-1);
   }   

      // check elevation mask
   if (minElev > 90 || minElev < 0)
   {
      cout  << "Enter an elevation mask between 0 and 90 degrees. Exiting" 
            << endl;
      exit(-1);
   }

      // verify we have a position
   if (rxPos.mag() < 1)
   {
      cout << "Did not find valid position. Exiting." << endl;
      exit(-1);
   }
      // dump ephemeris data if in debug mode
   if (debugLevel)
      ephReader.eph->dump(cout, debugLevel-1);

      // print debug info
   if (debugLevel || verboseLevel)
   {
      if (msid)
         cout << "MS ID: " << msid << endl;
         
      cout  << "Elevation mask: "      << minElev            << endl
            << "Ant position: "        << rxPos              << endl
            << "obsEpochMap.size() : " << obsEpochMap.size() << endl;
   }
}

void DOPCalc::process()
{
      // create an ephemeris store
   gpstk::XvtStore<SatID>& ephStore = *ephReader.eph;
   
      // output header
   cout << "\n>\tTime\t   # SVs    GDOP     PDOP     TDOP\n";

      // process obsEpochMap
   for (ObsEpochMap::const_iterator oemIter=obsEpochMap.begin(); 
        oemIter!=obsEpochMap.end(); 
        oemIter++)
   {
      DayTime tempTime  = oemIter->first;
      ObsEpoch obsEpoch = oemIter->second;
      
         // for each epoch, we need a vector of the visible satellites
      vector<SatID> satIDVec;
      
         // we also need a vector of the corresponding ranges
      vector<double> rangeVec;

         // procss obsEpoch
      for (ObsEpoch::const_iterator oeIter=obsEpoch.begin();
           oeIter != obsEpoch.end();
           oeIter++)
      {
         SatID satID = oeIter->first;
         SvObsEpoch svObsEpoch = oeIter->second;

            // try for SV info
         Xvt svXvt;
         try
         {
            svXvt = ephStore.getXvt(satID, tempTime);
         }
         catch(gpstk::Exception& e)
         {
            if (debugLevel > 2)
               cout << e << endl;
            continue;
         }

            // can now get elevation angle
         double elev = rxXvt.x.elvAngle(svXvt.x);         

            // output some info if in super-debug mode
         if (debugLevel > 3)
            cout  << "  " << tempTime << "  " << satID 
                  << " Elev: " << elev << " Azim: " 
                  << rxXvt.x.azAngle(svXvt.x) << endl;
         
            // skip this SV is it is below the elev mask
         if (elev < minElev)
            continue; 
         
         double rangeL1 = 0;
         double rangeL2 = 0;
         
            // see if we can find data for this SV
         SvObsEpoch::const_iterator obsEpochIter;
         for (obsEpochIter = svObsEpoch.begin(); 
              obsEpochIter != svObsEpoch.end(); 
              obsEpochIter++)
         {
            const ObsID& obsID = obsEpochIter->first;
            
               // look for a pseudorange from L1 (P, Y, or codeless)
            if (!(obsID.type == ObsID::otRange))
               continue;

            if (obsID.band == ObsID::cbL1 && 
                (obsID.code == ObsID::tcY ||
                 obsID.code == ObsID::tcP ||
                 obsID.code == ObsID::tcW ))
            {
               rangeL1 = obsEpochIter->second;
               continue;
            }
            
            if (obsID.band == ObsID::cbL2 && 
                (obsID.code == ObsID::tcY ||
                 obsID.code == ObsID::tcP ||
                 obsID.code == ObsID::tcW ))
            {
               rangeL2 = obsEpochIter->second;
               continue;
            }
         } // end loop over svObsEpoch
         
            // if have dual frequency data, do iono correction
         if (rangeL1 && rangeL2)
         {
            double ionoCorrection = 0;
            const double gamma = (L1_FREQ / L2_FREQ)*(L1_FREQ / L2_FREQ);
            ionoCorrection = 1./(1.-gamma)*(rangeL1-rangeL2);

               // store the SatID and corresponding range for the valid SV
            satIDVec.push_back(satID);
            rangeVec.push_back(rangeL1 - ionoCorrection);
              
            if (debugLevel > 2)
               cout << tempTime << " " << satID << " dual freq iono correction: "
                    << ionoCorrection  << "m , range: " << rangeL1 
                    << " corrected range: " << (rangeL1 - ionoCorrection) 
                    << endl;

         }
         
      }  // process obsEpoch
      
      if ( satIDVec.size() == 0)
         continue;
         
         // need a trop model pntr - points to the "void" model by default
      ZeroTropModel noTropModel;
      TropModel *tropPtr = &noTropModel;
         
         // use PRSolution class to get the covariance matrix
      PRSolution prSolution;
      prSolution.RAIMCompute(tempTime, satIDVec, rangeVec, ephStore, tropPtr);
         
         // use DOP class to compute DOPs
      DOP dop;
      dop.Compute(prSolution.Covariance);
        
         // output data
      cout << tempTime.printf("%4Y/%03j/%02H:%02M:%04.1f") 
           << fixed << right << setprecision(2)
           << " " << setw(3) << satIDVec.size()
           << " " << setw(8) << dop.GDOP
           << " " << setw(8) << dop.PDOP
           << " " << setw(8) << dop.TDOP
           << endl;
           
   }     // procss obsEpochMap
}        // void DOPCalc::process()

void DOPCalc::readObsFile(const CommandOptionWithAnyArg& obsFileOpt, 
                          ObsEpochMap &obsEpochMap)
{
      // read in obs data from each input file
   for (int i=0; i<obsFileOpt.getCount(); i++)
   {
      string fileName = obsFileOpt.getValue()[i];
      ObsReader obsReader(fileName, verboseLevel);

      if (msid)
         obsReader.msid = msid;

         // if we don't have a position yet and we were given a RINEX obs
         // file, then try to read the position from the header
      if (obsReader.inputType == FFIdentifier::tRinexObs)
      {
         rxPos = obsReader.roh.antennaPosition;
         
         if (debugLevel || verboseLevel)
            cout << "Tried reading position from RINEX header: "
                 << rxPos << endl;
      }
      
      while(obsReader())
      {
         if (!obsReader())
            break;
            
         ObsEpoch obsEpoch(obsReader.getObsEpoch());
         obsEpochMap[obsEpoch.time] = obsEpoch;
    
         if (debugLevel > 2)
            cout << obsEpoch << endl;         
      }
   }
}

int main(int argc, char *argv[])
{
   DOPCalc nada(argv[0]);

   if (!nada.initialize(argc, argv))
      exit(0);

   nada.run();
}
