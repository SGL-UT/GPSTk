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

#include "RinexObsData.hpp"
#include "RinexObsStream.hpp"
#include "RinexObsFilterOperators.hpp"

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
   gpstk::FileFilterFrameWithHeader<RinexObsStream, RinexObsData, RinexObsHeader>
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
   RinexObsHeader header1, header2;
   RinexObsStream ros1(inputFileOption.getValue()[0]), ros2(inputFileOption.getValue()[1]);

   ros1 >> header1;
   ros2 >> header2;

   if (header1.obsTypeList.size() != header2.obsTypeList.size())
   {
      cout << "The two files have a different number of observation types." << endl;
      cout << "The first file has ";
      vector<RinexObsType> types1 = header1.obsTypeList;
      vector<RinexObsType>::iterator i = types1.begin();
      while (i != types1.end())
      {
         cout << gpstk::RinexObsHeader::convertObsType(*i) << ' ';
         i++;
      }
      cout << endl;

      cout << "The second file has ";
      vector<RinexObsType> types2 = header2.obsTypeList;
      vector<RinexObsType>::iterator j = types2.begin();
      while (j != types2.end())
      {
         cout << gpstk::RinexObsHeader::convertObsType(*j) << ' ';
         j++;
      }
      cout << endl;
   }

   // find the obs data intersection
   RinexObsHeaderTouchHeaderMerge merged;

   merged(ff1.frontHeader());
   merged(ff2.frontHeader());

   cout << "Comparing the following fields (other header data is ignored):"
        << endl;
   set<RinexObsType> intersection = merged.obsSet;
   set<RinexObsType>::iterator m = intersection.begin();
   while (m != intersection.end())
   {
      cout << gpstk::RinexObsHeader::convertObsType(*m) << ' ';
      m++;
   }
   cout << endl;

   ff1.sort(RinexObsDataOperatorLessThanFull(intersection));
   ff2.sort(RinexObsDataOperatorLessThanFull(intersection));

   pair< list<RinexObsData>, list<RinexObsData> > difflist =
      ff1.diff(ff2, RinexObsDataOperatorLessThanFull(intersection));

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

   list<RinexObsData>::iterator firstitr = difflist.first.begin();
   if (verboseLevel)
      cout << "Differences of epochs in both files:" << endl;
   while (firstitr != difflist.first.end())
   {
      bool matched = false;
      list<RinexObsData>::iterator seconditr = difflist.second.begin();
      while ((!matched) && (seconditr != difflist.second.end()))
      {
         if (firstitr->time == seconditr->time)
         {
            RinexObsData::RinexSatMap::iterator fpoi, spoi;
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
               for (m = intersection.begin(); m != intersection.end(); m++)
               {
                  // no need to do a find, we're using the merged
                  // set of obses which guarantees that we have the
                  // obs in this record
                  double diff = (fpoi->second[*m]).data;
                  if (spoi != seconditr->obs.end())
                     diff -= (spoi->second[*m]).data;

                  cout << setw(14) << setprecision(3) << fixed << diff << ' '
                       << RinexObsHeader::convertObsType(*m) << ' ';

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

   list<RinexObsData>::iterator itr = difflist.first.begin();

   if (verboseLevel)
      cout << "Epochs only in first file:" << endl;
   while (itr != difflist.first.end())
   {
      (*itr).dump(cout << '<');
      itr++;
   }

   cout << endl;
   if (verboseLevel)
        cout << "Epochs only in second file:" << endl;
   itr = difflist.second.begin();
   while (itr != difflist.second.end())
   {
      (*itr).dump(cout << '>');
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
