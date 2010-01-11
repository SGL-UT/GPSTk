#pragma ident "$Id: $"

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


#ifndef MSCCALCULATOR_HPP
#define MSCCALCULATOR_HPP

#include <list>
#include <map>
#include <Exception.hpp>
#include <DayTime.hpp>
#include <ECEF.hpp>
#include "MSCData.hpp"

namespace gpstk
{
      /** @addtogroup fileutilsgroup MSN File Processing Tools */
      //@{

      /**
       * This class is an engine for determining the current position
       * of a Monitor Station given it's ECEF XYZ position at a point in
       * time and the continental drift velocity vectors.
       */
   class MSCCalculator
   {
   public:


         /// default constructor
      MSCCalculator() {}
      
         /**
          * constructor
          * @param msclist a list of MSCData objects
          */
      MSCCalculator(std::list<MSCData>& msclist) throw()
         { load(msclist); }
      
         /// loads the data in the msclist into the calculator
      MSCCalculator& load(std::list<MSCData>& msclist) throw();

         /**
          * return the station postion for the requested time
          * @param station the NIMA assigned number of the station of interest
          * @param time the time at which the position is required
          * @return an ECEF XYZ position for \c station valid at \c time
          * @throw gpstk::InvalidRequest no position could be determined for 
          *        \c station at \c time
          */
      gpstk::ECEF getPosition(unsigned long station,
                               const gpstk::DayTime& time) const
         throw(gpstk::InvalidRequest);

   private:

         /// contains all the MSCData objects keyed by station id
      std::map<unsigned long, std::list<MSCData> > mscmap;

         /// the number of seconds in a year
      static const unsigned long SEC_YEAR;
   }; // class MSCCalculator

      //@}

} // namespace gpstk

#endif
