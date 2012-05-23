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






#include "FileFilterFrameWithHeader.hpp"

#include "RinexMetData.hpp"
#include "RinexMetStream.hpp"
#include "RinexMetFilterOperators.hpp"

#include "DiffFrame.hpp"

using namespace std;
using namespace gpstk;

class RMWDiff : public DiffFrame
{
public:
   RMWDiff(char* arg0)
         : DiffFrame(arg0, 
                     std::string("RINEX Met"))
   {}

protected:
   virtual void process();
};


void RMWDiff::process()
{
   try
   {
      FileFilterFrameWithHeader<RinexMetStream, RinexMetData, RinexMetHeader>
         ff1(inputFileOption.getValue()[0]), ff2(inputFileOption.getValue()[1]);

         // find the obs data intersection
      RinexMetHeaderTouchHeaderMerge merged;

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
         exit(1);
      }

      merged(ff1.frontHeader());
      merged(ff2.frontHeader());

      set<RinexMetHeader::RinexMetType> intersection = merged.obsSet;

      cout << "Comparing the following fields (other header data is ignored):" << endl;
      set<RinexMetHeader::RinexMetType>::iterator m = intersection.begin();
      while (m != intersection.end())
      {
         cout << RinexMetHeader::convertObsType(*m) << ' ';
         m++;
      }
      cout << endl;

      if (timeOptions.getCount())
      {
         ff1.filter(RinexMetDataFilterTime(startTime, endTime));
         ff2.filter(RinexMetDataFilterTime(startTime, endTime));
      }

      ff1.sort(RinexMetDataOperatorLessThanFull(intersection));
      ff2.sort(RinexMetDataOperatorLessThanFull(intersection));

      pair< list<RinexMetData>, list<RinexMetData> > difflist = 
         ff1.diff(ff2, RinexMetDataOperatorLessThanFull(intersection));

      if (difflist.first.empty() && difflist.second.empty())
         exit(0);

      list<RinexMetData>::iterator firstitr = difflist.first.begin();
      while (firstitr != difflist.first.end())
      {
         bool matched = false;
         list<RinexMetData>::iterator seconditr = difflist.second.begin();
         while ((!matched) && (seconditr != difflist.second.end()))
         {
            if (firstitr->time == seconditr->time)
            {
               cout << setw(3) << firstitr->time.DOYday() << ' ' 
                    << setw(10) << setprecision(0)
                    << firstitr->time.DOYsecond() << ' ' 
                    << ff1.frontHeader().markerName << ' '
                    << ff2.frontHeader().markerName << ' ';

               for (m = intersection.begin(); m != intersection.end(); m++)
               {
                  double diff = firstitr->data[*m];
                  diff -= seconditr->data[*m];

                  cout << setw(7) << setprecision(1) << fixed << diff << ' '
                       << RinexMetHeader::convertObsType(*m) << ' ';

               }
               cout << endl;

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

      list<RinexMetData>::iterator itr = difflist.first.begin();
      while (itr != difflist.first.end())
      {
         (*itr).dump(cout << '<');
         itr++;
      }

      cout << endl;

      itr = difflist.second.begin();
      while (itr != difflist.second.end())
      {
         (*itr).dump(cout << '>');
         itr++;
      }

   }
   catch(Exception& e)
   {
      cout << e << endl
           << endl
           << "Terminating.." << endl;
   }
   catch(exception& e)
   {
      cout << e.what() << endl
           << endl
           << "Terminating.." << endl;
   }
   catch(...)
   {
      cout << "Unknown exception... terminating..." << endl;
   }
}


int main(int argc, char* argv[])
{
   try
   {
      RMWDiff m(argv[0]);
      if (!m.initialize(argc, argv))
         return 0;
      if (!m.run())
         return 1;
      
      return 0;
   }
   catch(Exception& e)
   {
      cout << e << endl;
   }
   catch(exception& e)
   {
      cout << e.what() << endl;
   }
   catch(...)
   {
      cout << "unknown error" << endl;
   }
   return 1;
}
