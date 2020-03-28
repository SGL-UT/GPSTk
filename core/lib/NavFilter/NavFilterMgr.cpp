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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#include "NavFilterMgr.hpp"

namespace gpstk
{
   NavFilterMgr ::
   NavFilterMgr()
   {
   }


   void NavFilterMgr ::
   addFilter(NavFilter* filt)
   {
      filters.push_back(filt);
   }


   NavFilter::NavMsgList NavFilterMgr ::
   validate(NavFilterKey* msgBits)
   {
      NavFilter::NavMsgList rv, newrv;
      rv.push_back(msgBits);
      rejected.clear();
      for (FilterList::iterator i = filters.begin(); i != filters.end(); i++)
      {
         if (rv.empty())
            break;
         (*i)->rejected.clear();
         newrv.clear();
         (*i)->validate(rv, newrv);
         if (!(*i)->rejected.empty())
            rejected.insert(*i);
         rv = newrv;
      }
      return rv;
   }


   NavFilter::NavMsgList NavFilterMgr ::
   finalize()
   {
         // final and intermediate return values
      NavFilter::NavMsgList rv, rv1, rv2;
         // current and next filter
      FilterList::iterator fliCur, fliNxt;
      rejected.clear();
         // touch ALL filters
      for (fliCur = filters.begin(); fliCur != filters.end(); fliCur++)
      {
            // finalize the data in the current filter
         (*fliCur)->rejected.clear();
         rv2.clear();
         (*fliCur)->finalize(rv2);
         
            // If the filter returned some data, we need to push it
            // into the next filter using validate.
         if (!rv2.empty())
         {
            fliNxt = fliCur;
            fliNxt++;
               // cascade the data through the end.
            rv1 = rv2;
            while ((fliNxt != filters.end()) && !rv1.empty())
            {
               (*fliNxt)->rejected.clear();
               rv2.clear();
               (*fliNxt)->validate(rv1, rv2);
               rv1 = rv2;
               fliNxt++;
            }
               // If the filter cascade got some data that passed all
               // filters, add it to the final return value.
            if (!rv1.empty())
            {
               std::copy(rv1.begin(), rv1.end(),
                         std::back_insert_iterator<NavFilter::NavMsgList>(rv));
            }
         }
      }
      return rv;
   }


   unsigned NavFilterMgr ::
   processingDepth()
      const noexcept
   {
      FilterList::const_iterator fli;
      unsigned rv = 1;
      for (fli = filters.begin(); fli != filters.end(); fli++)
      {
         rv += (*fli)->processingDepth();
      }
      return rv;
   }
}
