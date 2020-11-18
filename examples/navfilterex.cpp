//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#include <iostream>
#include <vector>
#include "NavFilterMgr.hpp"
#include "LNavFilterData.hpp"
#include "LNavParityFilter.hpp"
#include "LNavCookFilter.hpp"
#include "CommonTime.hpp"
#include "TimeString.hpp"
#include "StringUtils.hpp"

using namespace std;
using namespace gpstk;

int main()
{
   NavFilterMgr mgr;
   LNavCookFilter filtCook;
   LNavParityFilter filtParity;
   LNavFilterData navFiltData;
   std::string line, timeString, wordStr;
   CommonTime recTime;
      // Note that storing a single subframe is only valid in this
      // example because the filters in use immediately return the
      // data.
   std::vector<uint32_t> subframe(10,0);

   mgr.addFilter(&filtCook);
   mgr.addFilter(&filtParity);

      // point at what will be the first word when loaded
   navFiltData.sf = &subframe[0];

   while (cin)
   {
         // read nav data from stdin
      getline(cin, line);
      if (line[0] == '#')
         continue; // comment line
      if (line.length() == 0)
         continue; // blank line
      timeString = gpstk::StringUtils::firstWord(line, ',');
      scanTime(recTime, timeString, "%4Y %3j %02H:%02M:%04.1f");

         // copy the subframe words into subframe
      unsigned subframeIdx = 0;
      for (unsigned strWord = 6; strWord <= 15; strWord++)
      {
         wordStr = gpstk::StringUtils::word(line, strWord, ',');
         subframe[subframeIdx++] = gpstk::StringUtils::x2uint(wordStr);
      }
      navFiltData.prn = gpstk::StringUtils::asUnsigned(
         gpstk::StringUtils::word(line, 2, ','));
         // note that the test file contents use enums that probably
         // don't match ObsID's enums but that's really not important
         // for this example.
      navFiltData.carrier = (CarrierBand)gpstk::StringUtils::asInt(
         gpstk::StringUtils::word(line, 3, ','));
      navFiltData.code = (gpstk::TrackingCode)gpstk::StringUtils::asInt(
         gpstk::StringUtils::word(line, 4, ','));

         // validate the subframe
      NavFilter::NavMsgList l = mgr.validate(&navFiltData);
      NavFilter::NavMsgList::const_iterator nmli;
         // write any valid data to stdout
      for (nmli = l.begin(); nmli != l.end(); nmli++)
      {
         cout << timeString << ", 310, " << setw(2) << (*nmli)->prn << ", "
              << setw(1) << static_cast<int>((*nmli)->carrier) << ", "
              << static_cast<int>((*nmli)->code) << ", 1";
         LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*nmli);
         for (unsigned sfword = 0; sfword < 10; sfword++)
         {
            cout << ", " << hex << setiosflags(ios::uppercase) << setw(8)
                 << setfill('0') << fd->sf[sfword] << dec << setfill(' ');
         }
         cout << endl;
      }
   }

}
