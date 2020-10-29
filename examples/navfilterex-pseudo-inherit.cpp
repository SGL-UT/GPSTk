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

/** @file navfilterex-pseudo-container.cpp Pseudo-code for a
 * NavFilterMgr example where the application data is stored in a
 * container.  Refer to navfilterex.cpp for a functional example. */

/// application's navigation message storage class
class NavSubframe
{
public:
      /** The Navigation Subframe. 10 4-byte words.  There are 11
       * elements to facilitate access to elements 1-10. */
   std::vector<uint32_t> subframe;
};

/// Combine the nav filter store and the application store into a single class
class MyNavFilterData : public gpstk::LNavFilterData
{
public:
   NavSubframe appData;
};

/// function to read a single nav subframe
MyNavFilterData* readNav()
{
   if (!strm)
      return NULL;
   MyNavFilterData *rv = new MyNavFilterData();
   strm >> rv->appData;
   retun rv;
}

void filterEX()
{
      // Filter manager, where the work is done
   gpstk::NavFilterMgr mgr;
      // Individual filters being applied
   gpstk::LNavCookFilter filtCook;
   gpstk::LNavCrossSourceFilter filtVote;
   gpstk::NavFilter::NavMsgList::const_iterator nmli;

      // Tell the manager what filters to use
   mgr.addFilter(&filtCook);
   mgr.addFilter(&filtVote);

   while (strm)
   {
      MyNavFilterData *fd = readNav();

         // validate the subframe
      gpstk::NavFilter::NavMsgList l = mgr.validate(fd);

         // process the results
      for (nmli = l.begin(); nmli != l.end(); nmli++)
      {
         MyNavFilterData *ofd = dynamic_cast<MyNavFilterData*>(*nmli);
            // do something with fd (nav message that passed the filters)

            // Then free the memory.  Note that this would also free
            // the application's data, so you may want to delay this
            // depending on what you're trying to do.
         delete ofd;
      }
   }

      // Finalize the filters.  Probably not necessary when using only
      // depth 1 filters, but good practice in any case.
   gpstk::NavFilter::NavMsgList l = mgr.finalize();

      // process the results
   for (nmli = l.begin(); nmli != l.end(); nmli++)
   {
      MyNavFilterData *ofd = dynamic_cast<MyNavFilterData*>(*nmli);
         // do something with fd (nav message that passed the filters)

         // Then free the memory.  Note that this would also free
         // the application's data, so you may want to delay this
         // depending on what you're trying to do.
      delete ofd;
   }
}
