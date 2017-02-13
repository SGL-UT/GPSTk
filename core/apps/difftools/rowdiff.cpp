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

#include "FileFilterFrameWithHeader.hpp"

#include "Rinex3ObsData.hpp"
#include "Rinex3ObsStream.hpp"
#include "Rinex3ObsFilterOperators.hpp"

#include "DiffFrame.hpp"

#include "YDSTime.hpp"

using namespace std;
using namespace gpstk;

class ROWDiff : public DiffFrame
{
public:
      /// Input file does not exist exit code
   static const int EXIST_ERROR = 2;
      /// Differences found in input files
   static const int DIFFS_CODE = 1;
   ROWDiff(char* arg0)
      : DiffFrame(arg0,
                  std::string("RINEX Obs"))
   {}

protected:
   virtual void process();
};

void ROWDiff::process()
{
   gpstk::FileFilterFrameWithHeader<Rinex3ObsStream, Rinex3ObsData, Rinex3ObsHeader>
      ff1(inputFileOption.getValue()[0]), ff2(inputFileOption.getValue()[1]);

   // no data?  FIX make this program faster.. if one file
   // doesn't exist, there's little point in reading any.
   if (ff1.emptyHeader())
      cerr << "No header information for " << inputFileOption.getValue()[0]
           << endl;
   if (ff2.emptyHeader())
      cerr << "No header information for " << inputFileOption.getValue()[1]
           << endl;
   if (ff1.emptyHeader() || ff2.emptyHeader())
   {
      cerr << "Check that files exist." << endl;
      cerr << "diff failed." << endl;
      exitCode = EXIST_ERROR;
      return;
   }

   // determine whether the two input files have the same observation types
   Rinex3ObsHeader header1, header2;
   Rinex3ObsStream ros1(inputFileOption.getValue()[0]), ros2(inputFileOption.getValue()[1]);

   ros1 >> header1;
   ros2 >> header2;

   // find the obs data intersection

   if(header1.version != header2.version)
   {
      cout << "File 1 and file 2 are not the same RINEX version" << endl;
      // Reading a R2 file in translates/guesses its obsTypes into R3-style obsIDs,
      // but translating the R3 obsIDs to R2 is more likely to match.
      // So map R3 -> R2 then change the R2 header to match.
      if (header1.version < 3 && header2.version >= 3)
      {
         header2.prepareVer2Write();
         Rinex3ObsHeader::RinexObsVec r3ov;
         Rinex3ObsHeader::StringVec::iterator r2it = header1.R2ObsTypes.begin();
         while (r2it != header1.R2ObsTypes.end())
         {
            r3ov.push_back(header2.mapSysR2toR3ObsID["G"][*r2it]);
            r2it++;
         }
         header1.mapObsTypes["G"] = r3ov;
         ff1.frontHeader().mapObsTypes["G"] = r3ov;
      }
      else if (header2.version < 3 && header1.version >= 3)
      {
         header1.prepareVer2Write();
         Rinex3ObsHeader::RinexObsVec r3ov;
         Rinex3ObsHeader::StringVec::iterator r2it = header2.R2ObsTypes.begin();
         while (r2it != header2.R2ObsTypes.end())
         {
            r3ov.push_back(header1.mapSysR2toR3ObsID["G"][*r2it]);
            r2it++;
         }
         header2.mapObsTypes["G"] = r3ov;
         ff2.frontHeader().mapObsTypes["G"] = r3ov;
      }
   }

   // Find out what obs header 1 has that header 2 does/ doesn't have
   // add those to intersectionRom/ diffRom respectively.
   cout << "Comparing the following fields:" << endl;
   Rinex3ObsHeader::RinexObsMap diffRom;
   Rinex3ObsHeader::RinexObsMap intersectRom;
   for (Rinex3ObsHeader::RinexObsMap::iterator mit = header1.mapObsTypes.begin();
        mit != header1.mapObsTypes.end();
        mit++)
   {
      string sysChar = mit->first;
      cout << sysChar << ": ";
      for(Rinex3ObsHeader::RinexObsVec::iterator ID1 = mit->second.begin();
         ID1 != mit->second.end();
         ID1++)
      {
         try
         {
            header2.getObsIndex(sysChar, *ID1);
            intersectRom[sysChar].push_back(*ID1);
            cout << " " << ID1->asString();
         }
         catch(...)
         {
            diffRom[sysChar].push_back(*ID1);
         }
      }
      cout << endl;
   }

   // Find out what header 2 has that header 1 doesn't. Add them to diffRom
   for (Rinex3ObsHeader::RinexObsMap::iterator mit = header2.mapObsTypes.begin();
        mit != header2.mapObsTypes.end();
        mit++)
   {
      string sysChar = mit->first;
      for(Rinex3ObsHeader::RinexObsVec::iterator ID2 = mit->second.begin();
          ID2 != mit->second.end();
          ID2++)
      {
         try
         {
            header1.getObsIndex(sysChar, *ID2);
         }
         catch(...)
         {
            diffRom[sysChar].push_back(*ID2);
         }
      }
   }

   // Print out the differences between the obs in header1 and header2
   if(!diffRom.empty())
   {
      cout << "Ignoring unshared obs:" << endl;
      for (Rinex3ObsHeader::RinexObsMap::iterator mit = diffRom.begin();
           mit != diffRom.end();
           mit++)
      {
         string sysChar = mit->first;
         cout << sysChar << ": ";
         for (Rinex3ObsHeader::RinexObsVec::iterator ID = mit->second.begin();
              ID != mit->second.end();
              ID++)
         {
            cout << ID->asString() << " ";
         }
         cout << endl;
      }
   }

   std::list<Rinex3ObsData> a =
      ff1.halfDiff(ff2,Rinex3ObsDataOperatorLessThanFull(intersectRom));
   std::list<Rinex3ObsData> b =
      ff2.halfDiff(ff1, Rinex3ObsDataOperatorLessThanFull(intersectRom));

   pair< list<Rinex3ObsData>, list<Rinex3ObsData> > difflist =
      pair< list<Rinex3ObsData>, list<Rinex3ObsData> >( a, b);

   if (difflist.first.empty() && difflist.second.empty())
   {
      //Indicate to the user, before exiting, that rowdiff
      //performed properly and no differences were found.
      cout << "For the observation types that were compared, "
           << "no differences were found." << endl;
      exitCode = 0;
      return;
   }

      // differences found
   exitCode = DIFFS_CODE;

   list<Rinex3ObsData>::iterator firstitr = difflist.first.begin();
   if (verboseLevel)
      cout << "Differences of epochs in both files:" << endl;
   while (firstitr != difflist.first.end())
   {
      bool matched = false;
      list<Rinex3ObsData>::iterator seconditr = difflist.second.begin();
      while ((!matched) && (seconditr != difflist.second.end()))
      {
         if (firstitr->time == seconditr->time)
         {
            Rinex3ObsData::DataMap::iterator fpoi, spoi;
            for (fpoi = firstitr->obs.begin(); fpoi != firstitr->obs.end();
                 fpoi++)
            {
               cout << setw(3) << (static_cast<YDSTime>(firstitr->time)) << ' '
                    << setw(10) << setprecision(0)
                    << static_cast<YDSTime>(firstitr->time) << ' '
                    << ff1.frontHeader().markerName << ' '
                    << ff2.frontHeader().markerName << ' '
                    << setw(2) << fpoi->first << ' ';
               spoi = seconditr->obs.find(fpoi->first);
               Rinex3ObsHeader::RinexObsMap::iterator romIt;
               for (romIt = intersectRom.begin(); romIt != intersectRom.end(); romIt++)
               {
                  Rinex3ObsHeader::RinexObsVec::iterator ID;
                  for (ID = romIt->second.begin();
                       ID != romIt->second.end();
                       ID++)
                  {
                     // no need to do a find, we're using the merged
                     // set of obses which guarantees that we have the
                     // obs in this record
                     size_t fidx = header1.getObsIndex(romIt->first,*ID);
                     size_t sidx = header2.getObsIndex(romIt->first,*ID);
                     double diff = (fpoi->second[fidx]).data;
                     if (spoi != seconditr->obs.end())
                        diff -= (spoi->second[sidx]).data;

                     cout << setw(14) << setprecision(3) << fixed << diff << ' '
                     << ID->asString() << ' ';

                  }
               }
               cout << endl;
            }
            firstitr = difflist.first.erase(firstitr);
            seconditr = difflist.second.erase(seconditr);
            matched = true;
         }
         else
            seconditr++;
      }

      if (!matched)
         firstitr++;
   }

   list<Rinex3ObsData>::iterator itr = difflist.first.begin();
   if (verboseLevel)
      cout << "Epochs only in first file:" << endl;
   while (itr != difflist.first.end())
   {
      if (itr->obs.empty())
         return;

      cout << "<Dump of RinexObsData - time: ";
      cout << itr->timeString() << " epochFlag: "
      << " " << itr->epochFlag << " numSvs: " << itr->numSVs
      << fixed << setprecision(6)
      << " clk offset: " << itr->clockOffset << endl;
      if(itr->epochFlag == 0 || itr->epochFlag == 1)
      {
         Rinex3ObsHeader::RinexObsMap::const_iterator sysIt;
         for (sysIt = header1.mapObsTypes.begin();
              sysIt != header1.mapObsTypes.end(); sysIt++)
         {
            Rinex3ObsData::DataMap::iterator satIt;
            for(satIt = itr->obs.begin(); satIt != itr->obs.end(); satIt++)
            {
               cout << "Sat " << setw(2) << satIt->first;
               Rinex3ObsHeader::RinexObsVec::const_iterator obsIt;
               for (obsIt = sysIt->second.begin();
                    obsIt != sysIt->second.end(); obsIt++)
               {
                  RinexDatum datum = satIt->second[header1.getObsIndex(sysIt->first,*obsIt)];
                  cout << " " << obsIt->asString() << ":" << fixed <<
                  setprecision(3)
                  << " " << setw(13) << datum.data
                  << "/" << datum.lli << "/" << datum.ssi;
               }
               cout << endl;
            }
         }
      }
      else
      {
         cout << "aux. header info:\n";
         itr->auxHeader.dump(cout);
      }
      itr++;
   }

   cout << endl;

   itr = difflist.second.begin();
   if (verboseLevel)
        cout << "Epochs only in second file:" << endl;
   while (itr != difflist.second.end())
   {
      if (itr->obs.empty())
         return;

      cout << "<Dump of RinexObsData - time: ";
      cout << itr->timeString() << " epochFlag: "
      << " " << itr->epochFlag << " numSvs: " << itr->numSVs
      << fixed << setprecision(6)
      << " clk offset: " << itr->clockOffset << endl;
      if(itr->epochFlag == 0 || itr->epochFlag == 1)
      {
         Rinex3ObsHeader::RinexObsMap::const_iterator sysIt;
         for (sysIt = header2.mapObsTypes.begin();
              sysIt != header2.mapObsTypes.end(); sysIt++)
         {
            Rinex3ObsData::DataMap::iterator satIt;
            for(satIt = itr->obs.begin(); satIt != itr->obs.end(); satIt++)
            {
               cout << "Sat " << setw(2) << satIt->first;
               Rinex3ObsHeader::RinexObsVec::const_iterator obsIt;
               for (obsIt = sysIt->second.begin();
                    obsIt != sysIt->second.end(); obsIt++)
               {
                  RinexDatum datum = satIt->second[header2.getObsIndex(sysIt->first,*obsIt)];
                  cout << " " << obsIt->asString() << ":" << fixed <<
                  setprecision(3)
                  << " " << setw(13) << datum.data
                  << "/" << datum.lli << "/" << datum.ssi;
               }
               cout << endl;
            }
         }
      }
      else
      {
         cout << "aux. header info:\n";
         itr->auxHeader.dump(cout);
      }
      itr++;
   }
}

int main(int argc, char* argv[])
{
   try
   {
      ROWDiff m(argv[0]);
      if (!m.initialize(argc, argv))
         return m.exitCode;
      if (!m.run())
         return m.exitCode;

      return m.exitCode;
   }
   catch(Exception& e)
   {
      cout << e << endl;
   }
   catch(std::exception& e)
   {
      cout << e.what() << endl;
   }
   catch(...)
   {
      cout << "unknown error" << endl;
   }
      // only reach this point if an exception was caught
   return BasicFramework::EXCEPTION_ERROR;
}
