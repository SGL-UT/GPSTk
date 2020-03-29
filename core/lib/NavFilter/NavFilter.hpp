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

#ifndef NAVFILTER_HPP
#define NAVFILTER_HPP

#include <iterator>
#include <stdint.h>
#include <list>
#include "ObsID.hpp"
#include "NavFilterKey.hpp"

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Base class for all navigation message filters used by NavFilterMgr.
       */
   class NavFilter
   {
   public:
      typedef std::list<NavFilterKey*> NavMsgList;

      NavFilter();

         /** Validate/filter navigation messages.
          * This method is called by NavFilterMgr.  Messages are
          * ingested and filtered results returned, but not
          * necessarily all in a single call to this method. Multiple
          * calls may be required to accumulate sufficient data to
          * give meaningful results.  This behavior is
          * filter-specific.
          * @param[in,out] msgBitsIn A list of NavFilterKey* objects
          *   containing navigation messages.
          * @param[out] msgBitsOut The messages successfully passing
          *   the filter.  The data from msgBitsIn will not appear
          *   here until it successfully passes the filter, which may
          *   require multiple calls of validate with multiple epochs
          *   of data. */
      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut) = 0;

         /** Flush the filter's stored data, if any.  This method is
          * called by NavFilterMgr::finalize() which is in turn called
          * by the user.  This method allows any lingering data stored
          * internally by filters to be output.
          * @param[out] msgBitsOut The messages successfully passing
          *   the filter. */
      virtual void finalize(NavMsgList& msgBitsOut) = 0;

         /** Return the number of epochs worth of navigation data that
          * the filter child class stores internally to function
          * properly.  This can be used to determine the size of a nav
          * data buffer to allocate.  A return value of 0 means that
          * the subframes in msgBitsIn for the validate() call are
          * immediately put in msgBitsOut or in rejected.  A return
          * value of 1 means that nav subframes of time t will not be
          * accepted or rejected until validate is called with a nav
          * subframe of time t+1 or later, and so on.
          * Most filters will return a value of 0, indicating an
          * immediate validation of the data. */
      virtual unsigned processingDepth() const throw() = 0;

         /** Return a simple string containing the name of the filter
          * for the purposes of providing some user feedback as to
          * which filter rejected a message.  As an example,
          * LNavEmptyFilter would return "Empty".  The reason for
          * using this method instead of type_id.name() is that the
          * latter often returns compiler-munged names rather than
          * human-readable ones. */
      virtual std::string filterName() const throw() = 0;

         /// Debug support 
      virtual void dumpRejected(std::ostream& out) const; 

         /** Rejected nav messages go here.  If using NavFilterMgr,
          * this list will be cleared prior to the validate message
          * being called (to prevent memory bloat).
          *
          * Filter users may want to examine the contents of this list
          * after each NavFilterMgr::validate() call.
          *
          * @warning If you are NOT using NavFilterMgr, be aware that
          *   you will need to manage the rejected list yourself to
          *   avoid it growing unbounded. */
      NavMsgList rejected;

   protected:
         /** Add a validated nav msg to the output list.  This method
          * should be used by derived classes to pass validated
          * navigation message back to the NavFilterMgr user ONLY once
          * the nav data is no longer being internally stored by the
          * derived filter class. */
      inline void accept(NavFilterKey* data, NavMsgList& msgBitsOut);

         /** Add a list of validated nav messages to the output list.
          * This method should be used by derived classes to pass
          * validated navigation message back to the NavFilterMgr user
          * ONLY once the nav data is no longer being internally
          * stored by the derived filter class. */
      inline void accept(const NavMsgList& valid, NavMsgList& msgBitsOut);

         /** Add an invalid nav message to the reject list.  This
          * method should be used by derived classes to pass validated
          * navigation message back to the NavFilterMgr user ONLY once
          * the nav data is no longer being internally stored by the
          * derived filter class. */
      inline void reject(NavFilterKey* data);

         /** Add a list of invalid nav messages to the reject list.
          * This method should be used by derived classes to pass
          * validated navigation message back to the NavFilterMgr user
          * ONLY once the nav data is no longer being internally
          * stored by the derived filter class. */
      inline void reject(const NavMsgList& invalid);
   };

      //@}

   void NavFilter ::
   accept(NavFilterKey* data, NavMsgList& msgBitsOut)
   {
      msgBitsOut.push_back(data);
   }

   void NavFilter ::
   accept(const NavMsgList& valid, NavMsgList& msgBitsOut)
   {
      std::copy(valid.begin(), valid.end(),
                std::back_insert_iterator<NavMsgList>(msgBitsOut));
   }

   void NavFilter ::
   reject(NavFilterKey* data)
   {
      rejected.push_back(data);
   }

   void NavFilter ::
   reject(const NavMsgList& invalid)
   {
      std::copy(invalid.begin(), invalid.end(),
                std::back_insert_iterator<NavMsgList>(rejected));
   }

} // namepace gpstk

#endif // NAVFILTER_HPP
