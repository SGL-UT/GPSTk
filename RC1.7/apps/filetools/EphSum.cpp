#pragma ident "$Id$"
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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
#include "GPSEphemerisStore.hpp"

// fic
#include "FICStream.hpp"
#include "FICHeader.hpp"
#include "FICData.hpp"

#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"
#include "RinexNavHeader.hpp"

using namespace std;
using namespace gpstk;

class EphSum : public gpstk::BasicFramework
{
public:
   EphSum(const std::string& applName,
              const std::string& applDesc) throw();
   ~EphSum() {}
   virtual bool initialize(int argc, char *argv[]) throw();
   bool checkIOD( const gpstk::EngEphemeris ee, FILE* logfp );
   
protected:
   virtual void process();
   gpstk::CommandOptionWithAnyArg inputOption;
   gpstk::CommandOptionWithAnyArg outputOption;
   gpstk::CommandOptionWithAnyArg prnOption;
   gpstk::CommandOptionNoArg  	  xmitOption; // added 3/18/2009

   FILE *logfp;
   FILE *out2fp; 
   GPSEphemerisStore ges;
   
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
           xmitOption('x', "xmit", "List in order of transmission (default is TOE.", false)
{
   inputOption.setMaxCount(60);
   outputOption.setMaxCount(1);
   prnOption.setMaxCount(1);
   xmitOption.setMaxCount(1);
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
   int countByPRN[gpstk::MAX_PRN+1];
   for (int i1=0;i1<=gpstk::MAX_PRN+1;++i1) countByPRN[i1] = 0;
   
   bool successAtLeastOnce = false;
   
   vector<string> values;
   values = inputOption.getValue();
   for (int it=0;it<values.size();++it)
   {
      bool successOnThisFile = false;
         // Leave a record in the output file so we can verify what
         // ephemeris files were processed.
      fprintf(logfp,"# Processing input specification: %s",
                   values[it].c_str());
      // 
      // Try processing as a RINEX file
      try
      {
         RinexNavHeader rnh;
         RinexNavData rnd;
         
         RinexNavStream RNFileIn( values[it].c_str() );
         if (RNFileIn)
         {
            RNFileIn.exceptions(fstream::failbit);
            RNFileIn >> rnh;
         
            while (RNFileIn >> rnd)
            {
               EngEphemeris ee(rnd);
               ges.addEphemeris(ee);
            }
            successOnThisFile = true;
            fprintf(logfp," - Success(RINEX)\n");
         }
      }
      catch(gpstk::Exception& exc)
      {
         // do nothing
      }
      //if (successOnThisFile) cout << "Succeeded reading RINEX" << endl; 
         
         // If RINEX failed, try as FIC input file
      if (!successOnThisFile)
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
                  EngEphemeris ee(ficd);
                  ges.addEphemeris(ee);
                  recNum9++;
               }
               recNum++;
               //cout << "Processed rec#, rec9#  " << recNum << ", " << recNum9 << ", " << endl; 
            }
         }
         successOnThisFile = true;
         fprintf(logfp," - Success(FIC)\n");
         //if (successOnThisFile) cout << "Succeeded reading FIC" << endl; 
      }
      catch(gpstk::Exception& exc)
      {
         //cout << "Caught exception during FIC read." << endl;
         //cout << "Exception: " << exc << endl;
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
   
   string tform = "%04F %6.0g %02m/%02d/%02y %03j %02H:%02M:%02S";
   
   GPSEphemerisStore::EngEphMap eemap, eemapXmit;
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
      GPSEphemerisStore::EngEphMap::const_iterator ci;
      SatID sat = SatID( i, SatID::systemGPS);
      try
      {
         eemap = ges.getEphMap( sat );
      }
      catch(InvalidRequest)
      {
         // simply go on to the next PRN
         if (!singleSV || (singleSV && i==singlePRNID))
         {
            fprintf(logfp,"#\n");
            fprintf(logfp,"#PRN: %02d,  # of eph: NONE\n",i);
         }
         continue;
      }
      countByPRN[i] = eemap.size();
 
      if (singleSV && singlePRNID!=i) continue;
      
	  // sort by time of transmission (default is TOE) 
      if (xmitOption.getCount() > 0)
      {
         eemapXmit.clear();
         for (ci=eemap.begin(); ci!=eemap.end(); ++ci)
         {
            EngEphemeris ee = ci ->second;
            DayTime xmit = ee.getTransmitTime();
            eemapXmit.insert(make_pair(xmit,ee));
         } 
         
         eemap = eemapXmit;
      }
      

         // Header
      fprintf(logfp,"#\n");
      fprintf(logfp,"#PRN: %02d,  # of eph: %02d\n", i, (int) eemap.size());
      fprintf(logfp,"#PRN !               Xmit                !             Toe/Toc               !            End of Eff             !  IODC   Health\n");
      for (ci=eemap.begin(); ci!=eemap.end(); ++ci)
      {
         EngEphemeris ee = ci->second;

	    /*
	     * Calculating end of effectvity is a challenge.  IS-GPS-200 20.3.4.4
	     * states "The start of the transmission interval for each data set corresponds
	     * to the beginning of the curve fit interval for the data set."  HOWEVER,
	     * Table 20-XI Note 4 and Table 20-XII Note 5 state "The first data set of a
	     * new upload may be cut-in at any time and therefore the trasmission iterval
	     * may be less than the specified value."
	     *
	     * A new upload implies a two hour transmission interval and a 4 hour curve
	     * fit.  In addition, it has been emprically observed that the period of
	     * transmission for a new upload always starts prior to the Toe.  Therefore,
	     * if the Toe is NOT an even two-hour epoch AND the transmit time is 
	     * not an even two hour epoch, then it is likely the first ephemeris of a 
	     * new upload.  In such a case, the transmit time can be rounded BACK
	     * to the most recent even two hour epoch and considered the beginning time
	     * of effectivity for end of effectivity. 
	    */
         DayTime begEff = ee.getTransmitTime();
	 DayTime epochTime = ee.getEphemerisEpoch();
	 long TWO_HOURS = 7200;
         long epochRemainder = (long) epochTime.GPSsow() % TWO_HOURS;
	 long  xmitRemainder = (long) begEff.GPSsow() % TWO_HOURS;
	 if (epochRemainder != 0 && xmitRemainder != 0)
	 {
	    begEff = begEff - xmitRemainder;
	 }

	 short fitIntervalHours = ee.getFitInterval();
	 short ONE_HOUR = 3600;
	 DayTime endEff = begEff + ONE_HOUR * fitIntervalHours;

         fprintf(logfp,"  %02d ! %s ! %s ! %s ! 0x%03X  0x%02X %02d \n",
               i,
               ee.getTransmitTime().printf(tform).c_str(),
               ee.getEphemerisEpoch().printf(tform).c_str(),
               endEff.printf(tform).c_str(),
               ee.getIODC(),
               ee.getHealth(),
               ee.getHealth());
         //fprintf(logfp,"    |                                   | %s |\n",
         //      ee.getEpochTime().printf(tform).c_str());
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
 
