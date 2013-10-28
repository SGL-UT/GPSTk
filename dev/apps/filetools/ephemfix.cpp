// $Id$
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

// Removes ephemeris data that does not match almanac data from
// FIC and RINEX files. This was originally developed to fix
// files with data corrupted due to receiver PRN mistagging.

#include <iostream>
#include <cmath>

#include "BasicFramework.hpp"
#include "GPSAlmanacStore.hpp"
#include "AlmOrbit.hpp"
#include "FileFilterFrame.hpp"
#include "FICStream.hpp"
#include "FICHeader.hpp"
#include "FICData.hpp"
#include "FICFilterOperators.hpp"
#include "FICBase.hpp"
#include "FICStreamBase.hpp"
#include "RinexNavBase.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"
#include "StringUtils.hpp"

#include "FFIdentifier.hpp"

using namespace std;
using namespace gpstk;

// need an equality operator for EngEphemeris
namespace gpstk
{
   bool operator==(const gpstk::EngEphemeris& lhs,
                   const gpstk::EngEphemeris& rhs)
   {
         // PRN ID of SV
      if (lhs.getPRNID() != rhs.getPRNID()) return (false);
         // Clock epoch in GPS seconds of week
      if (::abs(lhs.getToc() - rhs.getToc()) > 1) return (false);
         // GPS week number for the ephemeris
      if (lhs.getFullWeek() != rhs.getFullWeek()) return (false);
         // IODE for the ephemeris
      if (lhs.getIODE() != rhs.getIODE()) return (false);
         // IODC for the PRN
      if (lhs.getIODC() != rhs.getIODC()) return (false);
      
      return true;
   }

}

class FICFixer : public BasicFramework
{
public:
   FICFixer(const string& applName) throw()
      : BasicFramework(
         applName,
         "Removes ephemeris data that does not match almanac data from FIC and RINEX files."),
        ephFileOpt('e',
                   "eph",
                   "File file to be scanned against provided almanac. This file"
                   "will not be altered. If errant ephemerides are found, a new file will be"
                   "created in the current directory that excludes the errors. "
                   "This option can be repeated for multiple files.", true),
        almFileOpt('a',
                   "almanac",
                   "FIC file that provides an almanac. Repeat for multiple files.",
                   true),
        tolerance(10000)
      
   {};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
   bool initialize(int argc, char *argv[]) throw();
#pragma clang diagnostic pop
protected:
   virtual void spinUp();

   virtual void process();
   
   virtual void shutDown(){};

private:

   CommandOptionWithAnyArg ephFileOpt, almFileOpt;
   GPSAlmanacStore almanacStore;
   list<EngEphemeris> badEngEphList;
   long tolerance;
   
   void scanFIC(const string& ficFilename);
   void scanRIN(const string& ficFilename);
};

bool FICFixer::initialize(int argc, char *argv[]) throw()
{
   if (!BasicFramework::initialize(argc,argv))
      return false;

   return true;
}

void FICFixer::spinUp()
{
   list<long> blockList;
   blockList.push_back(62);

   for (size_t i=0; i<almFileOpt.getCount(); i++ )
   {
      string almFilename = almFileOpt.getValue()[i];
      if (verboseLevel || debugLevel)
         cout << "Reading almanac data from " << almFilename << endl;
      FileFilterFrame<FICStream, FICData> fff(almFilename);
      fff.filter(FICDataFilterBlock(blockList));
      list<FICData>& ficList = fff.getData();
      if (debugLevel)
         cout << "Found " << ficList.size() << " block 62 records " << endl;
      for (list<FICData>::const_iterator itr = ficList.begin(); itr != ficList.end(); itr++)
      {
         AlmOrbit ao(*itr);
         almanacStore.addAlmanac(ao);
      }
   }
   
   // dump contents of almanac if in debug mode
   if (debugLevel > 2)
   {
      cout << "Contents of almanacStore:" << endl;
      almanacStore.dump(cout);
   }

   if ((almanacStore.getInitialTime() == CommonTime::BEGINNING_OF_TIME) ||
       (almanacStore.getFinalTime() == CommonTime::BEGINNING_OF_TIME))
   {
      cout << "No almanac data found. Exiting." << endl;
      exit(-1);
   }
   
   if (debugLevel || verboseLevel)
      cout << "Alm data initial time: " << almanacStore.getInitialTime() << endl
           << "Alm data final   time: " << almanacStore.getFinalTime()   << endl;         
}

void FICFixer::process()
{
   // scan and correct files
   for (size_t i=0;i<ephFileOpt.getCount(); i++)
   {
      string fn=ephFileOpt.getValue()[i];
      FFIdentifier id(fn);
      if (id == FFIdentifier::tRinexNav)
         scanRIN(fn);
      else if (id == FFIdentifier::tFIC)
         scanFIC(fn);
      else
         cout << "Can not process file of type " << FFIdentifier::describe(id)
              << ". Skipping " << fn << endl;
   }
}


void FICFixer::scanFIC(const string& fn)
{
   int n=StringUtils::numWords(fn, '/');
   string ofn=StringUtils::word(fn, n-1, '/');
      
   if (verboseLevel)
      cout << "Scanning " << fn << " saving to " << ofn << endl;

   //output file stream
   FICStream correctedFS(ofn.c_str(), ios::out|ios::binary);
   
   //input file stream
   FileFilterFrame<FICStream, FICData> fff(fn);
   list<FICData>& ficDataList = fff.getData();
   int numErrorsThisFile = 0;

   // capture the original header
   FICStream ifs(fn.c_str());
   FICHeader ifshdr;
   ifs >> ifshdr;
   correctedFS << ifshdr;

   list<FICData>::iterator itr;
   for (itr = ficDataList.begin(); itr != ficDataList.end(); itr++)
   {
      FICData& ficData = *itr;
      if (ficData.blockNum == 62 || ficData.blockNum == 162)
      {
         correctedFS << ficData;
         continue;
      }

      double magnitude = 0.0;
      EngEphemeris engEph;      
      short prn, iodc, iode;
      CommonTime ephEpoch;
      Xvt xvtEph, xvtAlm;
         
      if (ficData.blockNum == 9)
      {
         engEph = EngEphemeris(ficData);
         prn    = engEph.getPRNID();
         iodc   = engEph.getIODC();
         iode   = engEph.getIODE();
            
         SatID satID(prn, SatID::systemGPS);
         ephEpoch = engEph.getEphemerisEpoch();
         AlmOrbit almOrbit;
            
         try
         {
            almOrbit = almanacStore.findAlmanac(satID, ephEpoch);
         }
         catch (gpstk::Exception &exc)
         {
            if (verboseLevel)
               cout << "Have (block 9) ephemeris but no alm data for "  
                    << satID << " at " << ephEpoch << endl;
            // Keep data when we can't verify it.
            correctedFS << ficData;
            continue;
         }
            
         xvtEph = engEph.svXvt(ephEpoch);            
         xvtAlm = almOrbit.svXvt(ephEpoch);
         Triple difference = xvtEph.x - xvtAlm.x;
         magnitude = difference.mag();         
      }  // if (ficData.blockNum == 9)
      else if (ficData.blockNum == 109)
      {
         prn = ficData.i[1];
         int gpsWk = ficData.i[0];
         
         // tracking number is unimportant in this app, just pick 1
         short track = 1;
         engEph.addSubframe( &(ficData.i[2]),  gpsWk, prn, track);
         engEph.addSubframe( &(ficData.i[12]), gpsWk, prn, track);
         engEph.addSubframe( &(ficData.i[22]), gpsWk, prn, track);
            
         iodc = engEph.getIODC();
         iode = engEph.getIODE();
         ephEpoch = engEph.getEphemerisEpoch();
         SatID satID(prn, SatID::systemGPS);
         AlmOrbit almOrbit;
            
         try
         {
            almOrbit = almanacStore.findAlmanac(satID, ephEpoch);
         }
         catch (gpstk::Exception &exc)
         {
            if (verboseLevel)
               cout << "Have (block 109) ephemeris but no alm data for "  
                    << satID << " at " << ephEpoch << endl;
            // Keep data when we can't verify it.
            correctedFS << ficData;
            continue;
         }
         xvtEph = engEph.svXvt(ephEpoch);            
         xvtAlm = almOrbit.svXvt(ephEpoch);
         Triple difference = xvtEph.x - xvtAlm.x;
         magnitude = difference.mag();          

      }  // else if (ficData.blockNum == 109)
       
      if (magnitude < tolerance)
      {
         correctedFS << ficData; // keep data if it's OK
         continue;
      }

      // output info
      cout.fill(' ');
      cout << "File: " << fn << " : " << endl
           << "  Position Discrepancy: Block " << setw(3) << ficData.blockNum
           << ", Epoch: " << ephEpoch << ", PRN " << setw(2) << prn
           << ", IODC: 0x" << hex << setw(3) << setfill('0') << iodc 
           << ", IODE: 0x" << setw(3) << setfill('0') << iode << dec
           << ", diff = " << magnitude << " m" << endl;
         
      // try to find the "real" block copy
      for (list<FICData>::iterator itrInner = ficDataList.begin();
           itrInner != ficDataList.end();
           itrInner++)
      {
         FICData& ficDataTemp = *itrInner;
         double magTemp = 0.0;
         EngEphemeris engEphTemp;
           
         if (ficDataTemp.blockNum != ficData.blockNum)
            continue;
           
         if (ficDataTemp.blockNum == 9)
         {
            // check for clues that this could be the "real" block of data
              
            engEphTemp = EngEphemeris(ficDataTemp);              
               
            // prn must be different
            if (engEphTemp.getPRNID() == prn)
               continue;
               
            // time must be within 20 minutes
            CommonTime tempTime = engEphTemp.getEphemerisEpoch();
            CommonTime plus20min = ephEpoch + (60*20);
            CommonTime less20min = ephEpoch - (60*20);
            if (tempTime > plus20min)
               continue;
            else if (tempTime < less20min)
               continue;

            // SV location must be close
            Triple diff = xvtEph.x - engEphTemp.svXvt(tempTime).x;
            magTemp = diff.mag();               
            if (magTemp > 10)
               continue;
               
            // if we've gotten here then the suspect has been found
            numErrorsThisFile++;
            badEngEphList.push_back(engEph);
            // TODO come back and difference this eph XVT with the alm XVT
            cout << "  Possible match found in same FIC file: Epoch: "
                 << tempTime << ", PRN " << engEphTemp.getPRNID() 
                 << ", IODC: 0x" << hex << setw(3) << setfill('0') 
                 << engEphTemp.getIODC() << ", IODE: 0x" << setw(3) 
                 << setfill('0') << engEphTemp.getIODE()                    
                 << ", Position Diff: " << magTemp << " m" << dec << endl;
         }
         else if (ficDataTemp.blockNum == 109)
         {
            // check for clues that this could be the "real" block of data
             
            short prnTemp = ficDataTemp.i[1];
              
            // prn must be different
            if (prnTemp == prn)
               continue;
                  
            int gpsWkTemp = ficDataTemp.i[0];
               
            // need tracking number to form object, just pick 1
            short trackTemp = 1;
       
            // tracking number is unimportant in this app, just pick 1
            short track = 1;
#pragma unused(track)
            engEphTemp.addSubframe(&(ficDataTemp.i[2]),  gpsWkTemp,
                                   prnTemp, trackTemp);
            engEphTemp.addSubframe(&(ficDataTemp.i[12]), gpsWkTemp, 
                                   prnTemp, trackTemp);
            engEphTemp.addSubframe(&(ficDataTemp.i[22]), gpsWkTemp, 
                                   prnTemp, trackTemp);
               
            // time must be within 20 minutes
            CommonTime tempTime = engEphTemp.getEphemerisEpoch();
            CommonTime plus20min = ephEpoch + (60*20);
            CommonTime less20min = ephEpoch - (60*20);
            if (tempTime > plus20min)
               continue;
            else if (tempTime < less20min)
               continue;

            // SV location must be close
            Triple diff = xvtEph.x - engEphTemp.svXvt(tempTime).x;
            magTemp = diff.mag();               
            if (magTemp > 10)
               continue;            

            // if we've gotten here then the suspect has been found
            numErrorsThisFile++;
            badEngEphList.push_back(engEph);
            // TODO come back and difference this eph XVT with the alm XVT
            cout << "  Possible match found in same FIC file: Epoch: "
                 << tempTime << ", PRN " << engEphTemp.getPRNID()
                 << ", IODC: 0x" << hex << setw(3) << setfill('0') 
                 << engEphTemp.getIODC() << ", IODE: 0x" << setw(3) 
                 << setfill('0') << engEphTemp.getIODE()                    
                 << ", Position Diff: " << magTemp << "m" << dec << endl;
         }

      } // for (list<FICData>::iterator itrInner = ficDataList.begin()....

      // make sure we actually scanned something
      if (itr == ficDataList.begin())
      {
         cout << "Did not read any data from " << fn << endl;
         cout << "Exiting." << endl;
         exit(0);
      }
      
      if (numErrorsThisFile)
         cout << "Found " << numErrorsThisFile << " blocks with mistagged "
              << "PRNs that have duplicate blocks in " << fn << endl;

   }

   if (verboseLevel)
      cout << "Done scanning FIC file." << endl;
}

void FICFixer::scanRIN(const string& fn)
{
   int n=StringUtils::numWords(fn,'/');
   string ofn=StringUtils::word(fn, n-1, '/');
      
   if (verboseLevel)
      cout << "Scanning " << fn << " saving to " << ofn << endl;
   
   //output file stream
   RinexNavStream correctedRS(ofn.c_str(), ios::out|ios::binary);
   
   //input file stream
   FileFilterFrame<RinexNavStream,RinexNavData> fff(fn);
   list<RinexNavData>& rinNavDataList = fff.getData();
   int numErrorsThisFile = 0;

   // capture the original header
   RinexNavStream irs(fn.c_str());
   RinexNavHeader irshdr;
   irs >> irshdr;
   correctedRS << irshdr;

   if (verboseLevel || debugLevel)
      cout << "Scanning Rinex Nav file: " << fn << endl;
      
   list<RinexNavData>::iterator itr;

   for (itr = rinNavDataList.begin(); itr != rinNavDataList.end(); itr++)
   {
     RinexNavData& rinNavData = *itr;
     double magnitude = 0.0;
     EngEphemeris engEph;      
     short prn, iodc, iode;
     CommonTime ephEpoch;
     Xvt xvtEph, xvtAlm;
      
     engEph = EngEphemeris(rinNavData);
     prn    = engEph.getPRNID();
     iodc   = engEph.getIODC();
     iode   = engEph.getIODE();
            
     SatID satID(prn, SatID::systemGPS);
     ephEpoch = engEph.getEphemerisEpoch();
      
     AlmOrbit almOrbit;
            
     try
     {
        almOrbit = almanacStore.findAlmanac(satID, ephEpoch);
     }
     catch (gpstk::Exception &exc)
     {
        if (verboseLevel)
           cout << "Have ephemeris but no alm data for "  
                << satID << " at " << ephEpoch << endl;
        correctedRS << rinNavData; // keep data if it's OK
        continue;
     }

     xvtEph = engEph.svXvt(ephEpoch);            
     xvtAlm = almOrbit.svXvt(ephEpoch);
     Triple difference = xvtEph.x - xvtAlm.x;
     magnitude = difference.mag();         

     if (magnitude<tolerance)
     {
        correctedRS << rinNavData; // keep data if it's OK
        continue;
     }

     // output info
     cout.fill(' ');
     cout << "File: " << fn << " : " << endl
          << ", Epoch: " << CivilTime(ephEpoch) << ", PRN " << setw(2) << prn
          << ", IODC: 0x" << hex << setw(3) << setfill('0') << iodc 
          << ", IODE: 0x" << setw(3) << setfill('0') << iode << dec
          << ", diff = " << magnitude << " m" << endl;
         
     // try to find the "real" block copy
     for (list<RinexNavData>::iterator itrInner = rinNavDataList.begin();
          itrInner != rinNavDataList.end();
          itrInner++)
     {
        RinexNavData& rinNavDataTemp = *itrInner;
        double magTemp = 0.0;
        EngEphemeris engEphTemp;
             
        engEphTemp = EngEphemeris(rinNavDataTemp);              
               
        // prn must be different
        if (engEphTemp.getPRNID() == prn)
               continue;
               
        // time must be within 20 minutes
        CommonTime tempTime = engEphTemp.getEphemerisEpoch();
        CommonTime plus20min = ephEpoch + (60*20);
        CommonTime less20min = ephEpoch - (60*20);
        if (tempTime > plus20min)
           continue;
        else if (tempTime < less20min)
           continue;

        // SV location must be close
        Triple diff = xvtEph.x - engEphTemp.svXvt(tempTime).x;
        magTemp = diff.mag();               
        if (magTemp > 10)
           continue;
               
        // if we've gotten here then the suspect has been found
        numErrorsThisFile++;
        badEngEphList.push_back(engEph);
        // TODO come back and difference this eph XVT with the alm XVT
        cout << "  Possible match found in FIC file: Epoch: "
             << tempTime << ", PRN " << engEphTemp.getPRNID() 
             << ", IODC: 0x" << hex << setw(3) << setfill('0') 
             << engEphTemp.getIODC() << ", IODE: 0x" << setw(3) 
             << setfill('0') << engEphTemp.getIODE()                    
             << ", Position Diff: " << magTemp << "m\n" << dec;
     }// for (list<RinexNavData>::iterator itrInner = ficDataList.begin()....

     // make sure we actually scanned something
    if (itr == rinNavDataList.begin())
       cout << "Did not read any data from " << fn << endl;
      
   if (numErrorsThisFile)
      cout << "Found " << numErrorsThisFile << " blocks with mistagged "
           << "PRNs that have duplicate blocks in " << fn << endl;

   } // for (int i=0;ficFileOpt.getCount(); i++)  

   if (verboseLevel)
      cout << "Done scanning " << fn << endl;
}

int main(int argc, char *argv[])
{
   FICFixer nada(argv[0]);

   if (!nada.initialize(argc, argv))
      exit(0);

   nada.run();
}
