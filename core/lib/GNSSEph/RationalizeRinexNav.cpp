//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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
/// @file RationalizeRinexNav.cpp
#include "RationalizeRinexNav.hpp"

#include "GPSEphemeris.hpp"
#include "OrbElemRinex.hpp"
#include "SystemTime.hpp"
#include "TimeString.hpp"

namespace gpstk
{
   using namespace std;

   //----------------------------------------------------------------
   bool RationalizeRinexNav::inputFile(const std::string fn)
   {
      Rinex3NavStream rns(fn.c_str(), ios::in);
      if (!rns.is_open())
            return false;
      rns.exceptions(ifstream::failbit);

         // Collect header and store for later re-use
      Rinex3NavHeader rnh;
      rns >> rnh;
      rnhMap[fn] = rnh;

      TOC_LIST& tocList = listOfTocsByFile[fn];

      Rinex3NavData rnd;
      while (rns >> rnd)
      {
         NAV_DATA_LIST& ndl = sndl[rnd.sat];
         ndl.push_back(rnd);
    
            // Store a list of ToC values so 
            // we know which Toc values were in this file.
         NAV_ID_PAIR p = make_pair(gpstk::SatID(rnd.sat),rnd.time);
         tocList.push_back(p); 
      }    
      return true; 
   }

   //----------------------------------------------------------------
   bool RationalizeRinexNav::
   writeOutputFile(const std::string inFileName,
                   const std::string outFileName,
                   const std::string progName,
                   const std::string agencyName) const
   {
      map<string,Rinex3NavHeader>::const_iterator cft;
      cft = rnhMap.find(inFileName);
      if (cft==rnhMap.end()) 
         return false; 

         // Open a new Rinex 3 nav file and write the header
      Rinex3NavStream *rns = new Rinex3NavStream(outFileName.c_str(), ios::out|ios::trunc);
      Rinex3NavHeader rnh = cft->second;
      rnh.fileProgram = progName;
      rnh.fileAgency = agencyName;
      ostringstream ostr;
      ostr << CivilTime(SystemTime());
      rnh.date = ostr.str();
      *rns << rnh; 

      std::map<std::string,TOC_LIST>::const_iterator cft2;
      cft2 = listOfTocsByFile.find(inFileName);
      if (cft2==listOfTocsByFile.end())
         return false;
      const TOC_LIST& tList = cft2->second; 
      TOC_LIST::const_iterator cit;
      for (cit=tList.begin(); cit!=tList.end(); cit++)
      {
         const SatID& sidr = cit->first;
         const CommonTime& ct = cit->second; 

         SAT_NAV_DATA_LIST::const_iterator csndl;
         csndl = sndl.find(sidr);
         if (csndl==sndl.end())
            continue;
         const NAV_DATA_LIST& ndl = csndl->second;
         NAV_DATA_LIST::const_iterator cnt = ndl.begin();
         bool done = false;
         while (cnt!=ndl.end() && !done)
         {
            const Rinex3NavData& rnd = *cnt;
            if (rnd.time==ct)
            {
               *rns << rnd;
               done = true;
            }
            cnt++;
         }
      }
      rns->close();
   }

   //----------------------------------------------------------------
   void RationalizeRinexNav::rationalize()
      throw(InvalidRequest)
   {
         // These items store the status of the most recently observed
         // upload cutover
      bool prevTocOffset = false;
      bool prev2TocOffset = false;
      CommonTime prevToc;

         //  For each SV  
      SAT_NAV_DATA_LIST::iterator it1;
      for (it1=sndl.begin(); it1!=sndl.end(); it1++)
      {
            // This process is only relevant to GPS.  Therefore, 
            // if this is not a GPS SV, skip it.
         const SatID& sidr = it1->first;
         if (sidr.system!=SatID::systemGPS) continue;

         NAV_DATA_LIST& ndl = it1->second;

            // First pass:  
            // 1.) Fix the fit intervals
            // 2.) Find the non-upload data sets that can 
            // be trivially fixed as a result of the 
            // promises in 20.3.4.4.
         NAV_DATA_LIST::iterator it2;
         for (it2=ndl.begin();it2!=ndl.end();it2++)
         {
            Rinex3NavData& r3nd = *it2; 

            long sowToc = static_cast<GPSWeekSecond>(r3nd.time).sow;
            long origxmitTime = r3nd.xmitTime;
            CommonTime xmitCT = formXmitTime(r3nd);

               // RINEX specifcation allows fit interval to be expressed in hours 
               // or as a two-state identifier.  We want the fit interval consistently
               // represented in hours. 
               // If in hours, the valid values are
               // 4, 6, 8, 14, 26 (see IS-GPS-200 Table 20-XII).  
               // If an identifier, the valid values are 0 or 1.  0 stands for 4 hours
               // and 1 stands for 6 hours. 
            if (r3nd.fitint==0) 
            {
               r3nd.fitint=4;
               addLog(sidr,r3nd.time,"Fit set to 4 h");
            }
            if (r3nd.fitint==1)
            {
               r3nd.fitint=6;
               addLog(sidr,r3nd.time,"Fit set to 6 h");
            }

               // If Toc/Toe is an even-hour interval the initial time of transmission
               // will be Toc/Toe minus 1/2 of the fit interval. 
            if (sowToc%3600==0)
            {
               r3nd.xmitTime = sowToc - (r3nd.fitint/2 * 3600);
               r3nd.weeknum = static_cast<GPSWeekSecond>(r3nd.time).week;
               if (r3nd.xmitTime<0)
               {
                  r3nd.xmitTime += FULLWEEK;
                  r3nd.weeknum--; 
               } 
            }

               // Log changes
            long diff = r3nd.xmitTime - origxmitTime;
            if (diff!=0)
            {
               stringstream ss;
               ss << "xmitTime adjusted by " << diff << " s";
               addLog(sidr,r3nd.time,ss.str()); 
            }
         }

            // Get the data close to receive time order 
            // (as opposed to the Toc order that is typical in brdc files). 
         ndl.sort(compXmitTimes);

            // Second pass: 
            // There are still a couple of sort order issues 
            // with respect to the first data sets.
            //  
            // There's a limit to what can be done regarding the 
            // first data set in an upload.  The cutover can happen
            // at any time.  
            // However, it is safe to assume that the
            // cutover happens within a window between
            // two hours PRIOR to the Toc and the Toc.
         prevTocOffset = false;
         prev2TocOffset = false;

         for (it2=ndl.begin();it2!=ndl.end();it2++)
         {
            Rinex3NavData& r3nd = *it2; 

            long sowToc = static_cast<GPSWeekSecond>(r3nd.time).sow;
            long origXmitTime = r3nd.xmitTime;
            CommonTime xmitCT = formXmitTime(r3nd);

               // In some cases, the transmission time in the
               // Rinex nav file is slightly AFTER the Toc.   
               // So (arbitrarily) set the transmission time to five minutes
               // before the Toc (rounded to the nearest two hour interval).
            if (sowToc%3600!=0 && !prevTocOffset)
            {
               if (xmitCT>r3nd.time)
               {
                  short week = static_cast<GPSWeekSecond>(r3nd.time).week;
                  long twoHourOfWeek = sowToc / 7200;
                  long testTime = (twoHourOfWeek + 1) * 7200;
                  xmitCT = GPSWeekSecond(week,testTime);
                  xmitCT -= 300;
                  r3nd.xmitTime = static_cast<GPSWeekSecond>(xmitCT).sow;

                  stringstream ss;
                  ss << "Orig. xmitTime after Toc  ";
                  addLog(sidr,r3nd.time,ss.str()); 
               }
            }

               // It is also the case that the frist transmit time 
               // for an upload cutover must be sometime in the 
               // two hours preceding the time of the Toc (rounded
               // up to account for the small difference).  So if the
               // transmission time is before this, move it to be five minutes after
               // the preceding two-hour epoch.   NOTE: This is an absolute
               // fabrication, but the other options are 
               //  a.) Have the transmission times in the file out-of-order of
               //      transmission and wrong,
               //  b.) Omit this data set entirely.
            if (sowToc%3600!=0 && !prevTocOffset && !prev2TocOffset)
            {
               short week = static_cast<GPSWeekSecond>(r3nd.time).week;
               long twoHourOfWeek = sowToc / 7200;
               long testTime = (twoHourOfWeek) * 7200; 
               CommonTime prevTwoHourEpoch = GPSWeekSecond(week,testTime);
/*
               cout << "Found a 'too early' first data set.  PRN " << sidr
                    << " xmit: " << printTime(howCT,"%02H:%02M:%02S")
                    << " sowToc: " << printTime(r3nd.time,"%02H:%02M:%02S")
                    << " prevTwoHourEpoch: " << printTime(prevTwoHourEpoch,"%02H:%02M:%02S") << endl; 
*/
               if (xmitCT<prevTwoHourEpoch)
               {
                  xmitCT = prevTwoHourEpoch + 300;
                  r3nd.xmitTime = static_cast<GPSWeekSecond>(xmitCT).sow;

                  stringstream ss;
                  ss << "Orig. xmitTime too early. ";
                  addLog(sidr,r3nd.time,ss.str()); 
               }            
            }

            prev2TocOffset = prevTocOffset;
            if (sowToc%3600!=0) prevTocOffset = true;
             else prevTocOffset = false; 

            long diff = r3nd.xmitTime - origXmitTime;
            if (diff!=0)
            {
               stringstream ss;
               ss << "xmitTime adjusted by " << diff << " s";
               addLog(sidr,r3nd.time,ss.str()); 
            }
         }

            // Sort AGAIN so the upload cutover adjustments will be reflected
            // in the order of the data.
         ndl.sort(compXmitTimes);

            // Third pass:
            // By this time, it is HOPED that upload cutovers
            // and the following second data sets will be next
            // to each other in the sort order.
            //
            // If this is the SECOND data set of an upload, 
            // set the transmission time to be equivalent to the nominal beginning
            // of transmission based on the statements in IS-GPS-200
            // Section 20.3.4.5 and Table 20-XIII.
         prevTocOffset = false;
         prev2TocOffset = false;

         for (it2=ndl.begin();it2!=ndl.end();it2++)
         {
            Rinex3NavData& r3nd = *it2;

            long sowToc = static_cast<GPSWeekSecond>(r3nd.time).sow;
            long origXmitTime = r3nd.xmitTime;
            CommonTime xmitCT = formXmitTime(r3nd);

            bool secondDataSet = false;

            if (sowToc%3600!=0 && prevTocOffset)
               secondDataSet = true;

            if (secondDataSet)
            {
                  // The same "small offset" must be present.  Given an upload, 
                  // the spacing between data sets will be two hours
               double diff = r3nd.time - prevToc;
               if (diff==7200.0)
               {
                  // Set transmit time to most recent previous two-hour interval
                  // This assumes that the "small offset" was "back" in time. 
                  // We use the modulo to determine the two hour interval within the
                  // week and set the transmit time to the beginning of that period. 
                  long twoHourOfWeek = sowToc / 7200;
                  r3nd.xmitTime = twoHourOfWeek * 7200;
               }
            }

            long diff = r3nd.xmitTime - origXmitTime;
            if (diff!=0)
            {
               stringstream ss;
               ss << "xmitTime adjusted by " << diff << " s";
               addLog(sidr,r3nd.time,ss.str()); 
            }

               // Store Offset status for what will be TWO data sets back.
            prev2TocOffset = prevTocOffset;

               // Found a possible upload cutover.
               // Store it for later use.
            prevTocOffset = false;
            if (sowToc%3600!=0)
            {
               prevTocOffset = true;
               prevToc = r3nd.time;
            }
         }

            // Sort AGAIN so these final adjustments will be reflected
            // in the order of the data.
         ndl.sort(compXmitTimes);
      }
   }

   //----------------------------------------------------------------
   bool RationalizeRinexNav::loadStore(OrbitEphStore& oes)
   {
      mostRecentLoadCount = 0; 
      stringstream ss; 
      bool retVal = true;

      SAT_NAV_DATA_LIST::const_iterator cit1;
      for (cit1=sndl.begin(); cit1!=sndl.end(); cit1++)
      {
         const SatID& sidr = cit1->first;
         const NAV_DATA_LIST& ndl = cit1->second;

         NAV_DATA_LIST::const_iterator cit2;
         for (cit2=ndl.begin();cit2!=ndl.end();cit2++)
         {
            const Rinex3NavData& r3nd = *cit2; 
            switch (r3nd.sat.system)
            {
               case SatID::systemGPS:
               {
                  try
                  {
                     GPSEphemeris oe(r3nd);
                     oes.addEphemeris(dynamic_cast<OrbitEph*>(&oe)); 
                     mostRecentLoadCount++;
                  }
                  catch (InvalidParameter ip)
                  {
                     ss << ip.getText() << endl;
                     retVal = false;
                  }
                  break;
               }

               // Add other systems
               
               default:
                  break;
            }
         }
      }
      mostRecentLoadErrors = ss.str();
      return retVal; 
   }

  //----------------------------------------------------------------
   bool RationalizeRinexNav::loadStore(OrbElemStore& oes)
   {
      mostRecentLoadCount = 0; 
      bool retVal = true;
      stringstream ss; 

      SAT_NAV_DATA_LIST::const_iterator cit1;
      for (cit1=sndl.begin(); cit1!=sndl.end(); cit1++)
      {
         const SatID& sidr = cit1->first;
         const NAV_DATA_LIST& ndl = cit1->second;

         NAV_DATA_LIST::const_iterator cit2;
         for (cit2=ndl.begin();cit2!=ndl.end();cit2++)
         {
            const Rinex3NavData& r3nd = *cit2; 
            OrbElemBase* oep; 

            switch (r3nd.sat.system)
            {
               case SatID::systemGPS:
               {
                  try
                  {
                     OrbElemRinex oe(r3nd);
                     oes.addOrbElem(dynamic_cast<OrbElemBase*>(&oe)); 
                     mostRecentLoadCount++;
                  }
                  catch (InvalidParameter ip)
                  {
                     ss << ip.getText() << endl;
                     retVal = false;
                  }
                  break;
               }

               // Add other systems
               
               default:
                  break;
            }
        }
      }
      mostRecentLoadErrors = ss.str();
      return retVal; 
   }

   //----------------------------------------------------------------
   std::string RationalizeRinexNav::
   getLoadErrorList() const {return mostRecentLoadErrors;}

   //----------------------------------------------------------------
   unsigned long RationalizeRinexNav::
   getNumLoaded() const {return mostRecentLoadCount;}

   //----------------------------------------------------------------
   void RationalizeRinexNav::dump(std::ostream& out) const
   {
      SAT_NAV_DATA_LIST::const_iterator cit1;
      for (cit1=sndl.begin(); cit1!=sndl.end(); cit1++)
      {
         const SatID& sidr = cit1->first;
         const NAV_DATA_LIST& ndl = cit1->second;

         out << endl << "Nav Data Sets for " << sidr << endl;
         out << "---------------------------------------------" << endl;
         out << "!              xmit                   !            Toc          !     !" << endl;
         out << "!   SOW  week mm/dd/yyyy DOY hh:mm:ss ! mm/dd/yyyy DOY hh:mm:ss ! fit !" << endl;

         NAV_DATA_LIST::const_iterator cit2;
         for (cit2=ndl.begin();cit2!=ndl.end();cit2++)
         {
            const Rinex3NavData& r3nd = *cit2; 
            out << strSumm(r3nd) << endl;
         }
      }
   }

   //----------------------------------------------------------------
   void RationalizeRinexNav::outputActionLog(std::ostream& out) const
   {
      SAT_NAV_DATA_LIST::const_iterator cit1;
      for (cit1=sndl.begin(); cit1!=sndl.end(); cit1++)
      {
         const SatID& sidr = cit1->first;
         const NAV_DATA_LIST& ndl = cit1->second;

         SAT_LOG_DATA_LIST::const_iterator citLog;
         citLog = sldl.find(sidr);

         out << endl << "Nav Data Sets for " << sidr << endl;
         out << "---------------------------------------------" << endl;
         out << "!              xmit                   !        Toc              !     !" << endl;
         out << "!   SOW  week mm/dd/yyyy DOY hh:mm:ss ! mm/dd/yyyy DOY hh:mm:ss ! fit ! Action" << endl;

         NAV_DATA_LIST::const_iterator cit2;
         for (cit2=ndl.begin();cit2!=ndl.end();cit2++)
         {
            const Rinex3NavData& r3nd = *cit2; 
            out << strSumm(r3nd);
            if (citLog!=sldl.end())
            {
               const LOG_DATA_LIST& logMap = citLog->second; 
               LOG_DATA_LIST::const_iterator citLog2;
               citLog2 = logMap.find(r3nd.time);
               if (citLog2!=logMap.end())
               {
                  const string& s = citLog2->second;
                  out << s;
               }
            }
            out << endl;
         }
      }
   }

   //----------------------------------------------------------------
   void RationalizeRinexNav::clear()
   {
          // Clear the navigation message data
      SAT_NAV_DATA_LIST::iterator it1;
      for (it1=sndl.begin(); it1!=sndl.end(); it1++)
      {
         NAV_DATA_LIST& ndl = it1->second;
         ndl.clear();
      }
      sndl.clear();

          // Clear the information regarding the actions taken
      SAT_LOG_DATA_LIST::iterator it3;
      for (it3=sldl.begin(); it3!=sldl.end(); it3++)
      {
         LOG_DATA_LIST& ldl = it3->second;
         ldl.clear();
      }
      sldl.clear();
   }

   //----------------------------------------------------------------
   std::string RationalizeRinexNav::strSumm(const Rinex3NavData& r3nd) const
   {
      string tform="%02m/%02d/%04Y %03j %02H:%02M:%02S";
      stringstream ss;

         // Have to figure out the appropriate week for the transmission time
      CommonTime xmitCT = formXmitTime(r3nd);
      int xmitWeek = static_cast<GPSWeekSecond>(xmitCT).week;
      ss << "!" << setw(6) << r3nd.xmitTime << "  " << setw(4) << xmitWeek << " " ;
      ss << printTime(xmitCT,tform) << " ! ";
      ss << printTime(r3nd.time,tform) << " ! ";
      ss << setw(3) << r3nd.fitint << " ! "; 
      return(ss.str());
   }

   //----------------------------------------------------------------
   void RationalizeRinexNav::addLog(const SatID& sidr, const CommonTime& ct, const string& s)
   {
      LOG_DATA_LIST& ldl = sldl[sidr];

      LOG_DATA_LIST::iterator it;
      it = ldl.find(ct);
      if (it==ldl.end())
         ldl[ct] = s;
      else    
      {
         string& currString = it->second;
         currString += ", " + s; 
      }      
   }

   //----------------------------------------------------------------
   bool RationalizeRinexNav::compXmitTimes(const Rinex3NavData& left, const Rinex3NavData& right)
   {
      CommonTime leftCT = formXmitTime(left);
      CommonTime rightCT = formXmitTime(right);

      return (leftCT<rightCT);
   }

   //----------------------------------------------------------------
   CommonTime RationalizeRinexNav::formXmitTime(const Rinex3NavData& r3nd)
   {
      int weekEpoch = static_cast<GPSWeekSecond>(r3nd.time).week;
      int sowEpoch = static_cast<GPSWeekSecond>(r3nd.time).sow; 

      int xmitWeek = weekEpoch;
      long diff = sowEpoch - r3nd.xmitTime;
      if (diff<-HALFWEEK)
         xmitWeek--; 

      CommonTime xmitCT = GPSWeekSecond(xmitWeek, r3nd.xmitTime);
      return(xmitCT);
   }

}  // End of namespace gpstk
