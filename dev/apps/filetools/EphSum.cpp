/**
*  Given a PRN ID and a date (DOY, Year), one or more navigation
*  message data file(s) and assemble a summary of all ephemerides relevant
*  to the day for the PRN.  Display the summary as a one-line-per-ephemeris
*  data set that shows the transmit time, the time of effectivity, the end
*  of effectivity, the IODC, and the health.
*  
*/
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
//  Copyright 2009, The University of Texas at Austin
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
// System
#include <stdio.h>

// gpstk
#include "FileFilterFrame.hpp"
#include "BasicFramework.hpp"
#include "StringUtils.hpp"
#include "CommonTime.hpp"
#include "TimeString.hpp"
#include "GPSWeekSecond.hpp"

// OrbElem
#include "OrbElemFIC9.hpp"
#include "OrbElemRinex.hpp"
#include "GPSOrbElemStore.hpp"

// fic
#include "FICStream.hpp"
#include "FICHeader.hpp"
#include "FICData.hpp"

#include "Rinex3NavStream.hpp"
#include "Rinex3NavData.hpp"
#include "Rinex3NavHeader.hpp"

using namespace std;
using namespace gpstk;

class EphSum : public gpstk::BasicFramework
{
public:
   EphSum(const std::string& applName,
              const std::string& applDesc) throw();
   ~EphSum() {}
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
   virtual bool initialize(int argc, char *argv[]) throw();
   bool checkIOD( const gpstk::EngEphemeris ee, FILE* logfp );
#pragma clang diagnostic pop
protected:
   virtual void process();
   gpstk::CommandOptionWithAnyArg inputOption;
   gpstk::CommandOptionWithAnyArg outputOption;
   gpstk::CommandOptionWithAnyArg prnOption;
   gpstk::CommandOptionNoArg      noRationalizeOption;

   FILE *logfp;
   FILE *out2fp; 
   GPSOrbElemStore ges;
   
   bool readARinexFile;
   int numFICErrors;
};

int main( int argc, char*argv[] )
{
   try
   {
      string applDesc = "\nSummarize contents of a navigation message file.  "
        "EphSum works on either RINEX navigation message files or FIC files.  "
        "The summary is in a text output file.  The summary contains the transmit "
        "time, time of effectivity, end of effectivity, IODC, and health as a "
        "one-line-per ephemeris summary.  The number of ephemerides found per SV "
        "is also provided.  The number of ephemerides per SV is also summarized at the end.  "
        "The default is to summarize all SVs found. If a specific PRN ID is provided, "
        "only data for that PRN ID will be sumarized.";
      EphSum fc("EphSum", applDesc);
      if (!fc.initialize(argc, argv)) return(false);
      fc.run();
   }
   catch(gpstk::Exception& exc)
   {
      cout << exc << endl;
      return 1;
   }
   catch(...)
   {
      cout << "Caught an unnamed exception. Exiting." << endl;
      return 1;
   }
   return 0;
}

EphSum::EphSum(const std::string& applName, 
                       const std::string& applDesc) throw()
          :BasicFramework(applName, applDesc),
           inputOption('i', "input-file", "The name of the navigation message file(s) to read.", true),
           outputOption('o', "output-file", "The name of the output file to write.", true),
           prnOption('p', "PRNID","The PRN ID of the SV to process (default is all SVs)",false),
           noRationalizeOption('n',"noRat","Do not rationalize the data store",false),
           readARinexFile(false)
{
   inputOption.setMaxCount(60);
   outputOption.setMaxCount(1);
   prnOption.setMaxCount(1);
   noRationalizeOption.setMaxCount(1);
   numFICErrors = 0;
}

bool EphSum::initialize(int argc, char *argv[])
   throw()
{
   if (!BasicFramework::initialize(argc, argv)) return false;
   
   if (debugLevel)
   {
      cout << "Input File(s):  " << inputOption.getValue().front() << endl;
      cout << "Output File: " << outputOption.getValue().front() << endl;
      cout << "PRN ID     : "; 
      if (prnOption.getCount()==0)
      {
         cout << "all" <<  endl;    
      }
      else
      {
         cout << "PRN ID     : " <<    prnOption.getValue().front() << endl;
      }
   }
   
      // Open the output file
   logfp = fopen( outputOption.getValue().front().c_str(),"wt");
   if (logfp==0) 
   {
      cout << "Failed to open output file. Exiting." << endl;
      return 1;
   }
   fprintf(logfp,"# Output file from EphSum\n");

   return true;   
}

void EphSum::process()
{
   string tform = "%04F %6.0g %02m/%02d/%02y %03j %02H:%02M:%02S";
   
   int countByPRN[gpstk::MAX_PRN+1];
   for (int i1=0;i1<=gpstk::MAX_PRN;++i1) countByPRN[i1] = 0;
   
   bool successAtLeastOnce = false;
   
   vector<string> values;
   values = inputOption.getValue();
   for (size_t it=0;it<values.size();++it)
   {
      bool successOnThisFile = false;
         // Leave a record in the output file so we can verify what
         // ephemeris file(s) were processed.
      fprintf(logfp,"# Processing input specification: %s",
                   values[it].c_str());
      // 
      // Try processing as a RINEX file
      /* debug */
      fprintf(logfp,"\n");
      try
      {
         Rinex3NavHeader rnh;
         Rinex3NavData rnd;
         
         Rinex3NavStream RNFileIn( values[it].c_str() );
         if (RNFileIn)
         {
            RNFileIn.exceptions(fstream::failbit);
            RNFileIn >> rnh;

            int recNum = 0;          
            while (RNFileIn >> rnd)
            {
               OrbElemRinex oer(rnd);
         /* debug          
         fprintf(logfp,"  %02d ! %s ! %s ! %s ! %s ! 0x%03X  0x%02X %02d \n",
               oer.satID.id,
               printTime(oer.beginValid,tform).c_str(),
               printTime(oer.transmitTime,tform).c_str(),
               printTime(oer.ctToe,tform).c_str(),
               printTime(oer.endValid,tform).c_str(),
               oer.IODC,
               oer.health,
               oer.health);
          /* end debug */
          
               ges.addOrbElem(oer);
               recNum++;
               readARinexFile = true;         // At least read one record successfully
               if (debugLevel)
               {
                  cout << "Processed rec#  " << recNum << endl;
               }
            }
            successOnThisFile = true;
            fprintf(logfp," - Success(RINEX)\n");
         }
      }
      catch(gpstk::Exception& exc)
      {
         if (debugLevel)
         {
            cout << "Caught exception during Rinex read." << endl;
            cout << "Exception: " << exc << endl;
         }
         // do nothing
      }
      if (debugLevel && successOnThisFile) cout << "Succeeded reading RINEX" << endl; 
         
         // If RINEX failed, try as FIC input file
      if (!readARinexFile)
      {
      try
      {
         FICHeader fich;
         FICData ficd;
         
         FICStream FICFileIn( values[it].c_str() );
         if (FICFileIn)
         {
            FICFileIn.exceptions(fstream::failbit);
            FICFileIn >> fich;
         
            int recNum = 0;
            int recNum9 = 0;
            while (FICFileIn >> ficd)
            {
               if (ficd.blockNum==9)
               {
                  OrbElemFIC9 oef(ficd);
                  ges.addOrbElem(oef);
                  recNum9++;
               }
               recNum++;
               if (debugLevel)
               {
                  cout << "Processed rec#, rec9#  " << recNum << ", " << recNum9 << ", " << endl; 
               }
            }
         }
         successOnThisFile = true;
         fprintf(logfp," - Success(FIC)\n");
         if (debugLevel && successOnThisFile) cout << "Succeeded reading FIC" << endl; 
      }
      catch(gpstk::Exception& exc)
      {
         if (debugLevel)
         {
            cout << "Caught exception during FIC read." << endl;
            cout << "Exception: " << exc << endl;
         }
         // do nothing
      }
      }
      if (successOnThisFile) successAtLeastOnce = true;
       else fprintf(logfp," - FAILURE\n");
   }
   
   if (!successAtLeastOnce)
   {
      cout << "Read no ephemeris data." << endl;
      cout << "EphSum will terminate." << endl;
      exit(1);
   }
   
   //string tform = "%04F %6.0g %02m/%02d/%02y %03j %02H:%02M:%02S";

   if (debugLevel)
   {
      cout << "Above decision branch for rationalize()." << endl;
   }
   if (noRationalizeOption.getCount()==0)
   {
      if (debugLevel)
      {
         cout << "Calling rationlize()" << endl;
      }
      ges.rationalize();
      if (debugLevel)
      {
         cout << "Back from rationlize()" << endl;
      }
   }
   
   map<CommonTime, GPSEphemeris> eemap;
   map<CommonTime, GPSEphemeris>::const_iterator ci;
   long maxprn = gpstk::MAX_PRN;
   
   bool singleSV = false; 
   int singlePRNID = 0;
   if (prnOption.getCount()>0)
   {
      singlePRNID = StringUtils::asInt(prnOption.getValue().front());
      singleSV = true;
   }
   for (int i=1;i<=maxprn;++i) 
   {
      SatID sat = SatID( i, SatID::systemGPS);

      try
      {
            // get list for this prn
         const GPSOrbElemStore::OrbElemMap& oemap = ges.getOrbElemMap(sat);

         int sizeprn = oemap.size();
         countByPRN[i] = sizeprn;
 
         if (singleSV && singlePRNID!=i) continue;
      
            // Header
         fprintf(logfp,"#\n");
         fprintf(logfp,"#PRN: %02d,  # of eph: %02d\n", i, sizeprn);
         fprintf(logfp,"#PRN !           Begin Valid             ");
         fprintf(logfp,     "!               Xmit                ");
         fprintf(logfp,     "!             Toe/Toc               ");
         fprintf(logfp,     "!            End of Eff             ");
         fprintf(logfp,     "!  IODC   Health\n");

         GPSOrbElemStore::OrbElemMap::const_iterator ci;
         for (ci=oemap.begin();ci!=oemap.end();ci++)
         {
            const OrbElem* ee = ci->second;
            if (readARinexFile)
            {
               const OrbElemRinex* eel = dynamic_cast<const OrbElemRinex*>(ee); 
   
               fprintf(logfp,"  %02d ! %s ! %s ! %s ! %s ! 0x%03X  0x%02d %02d \n",
                     eel->satID.id,
                     printTime(eel->beginValid,tform).c_str(),
                     printTime(eel->transmitTime,tform).c_str(),
                     printTime(eel->ctToe,tform).c_str(),
                     printTime(eel->endValid,tform).c_str(),
                     eel->IODC,
                     eel->health,eel->health);
            }
            else  // for FIC
            {
               const OrbElemLNav* eel = dynamic_cast<const OrbElemLNav*>(ee); 
   
               fprintf(logfp,"  %02d ! %s ! %s ! %s ! %s ! 0x%03X  0x%02d %02d \n",
                     eel->satID.id,
                     printTime(eel->beginValid,tform).c_str(),
                     printTime(eel->transmitTime,tform).c_str(),
                     printTime(eel->ctToe,tform).c_str(),
                     printTime(eel->endValid,tform).c_str(),
                     eel->IODC,
                     eel->health,eel->health);
            }
         }
      }
      catch(InvalidRequest exc)
      {
         countByPRN[i] = 0;
         continue;
      }
   }
   
   fprintf(logfp,"#\n#Summary of Counts by PRN\n");
   fprintf(logfp,"# PRN    Count\n");
   for (int i2=1;i2<=gpstk::MAX_PRN;++i2)
   {
      fprintf(logfp,"#  %02d    %5d\n",i2,countByPRN[i2]);
   }
   
   if (debugLevel) cout << "done." << endl;
}
 
