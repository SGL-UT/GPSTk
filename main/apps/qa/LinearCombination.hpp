#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/receiver/RinexObsRoller.hpp#1 $"

/**
 * @file LinearCombination.hpp
 * Interprets an expression of linear combinations of RINEX observables.
 * Class declarations.
 */

#ifndef GPSTK_LINEAR_COMBINATION_HPP
#define GPSTK_LINEAR_COMBINATION_HPP

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

#include <ostream>
#include <fstream>
#include <string>
#include <map>

#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "Exception.hpp"
#include "DayTime.hpp"

namespace gpstk
{
   /** @defgroup MeasurementQuality */
   //@{

   typedef std::map<RinexPrn,double> EpochCombination;
   
   class LinearCombination
   {
   public:

      /// Constructor
      LinearCombination(const std::string& expression );
      
      /// Destructor
      virtual ~LinearCombination(void);

         /**
          * Applies the linear combination to an epoch's worth of observations.
          */
      EpochCombination evaluate(const RinexObsData& rod); 

      void addCoefficient(RinexObsHeader::RinexObsType index, 
                          double coefficient)
      {
         coeffList[index]=coefficient;
      };
      
   protected:

   private:

      std::map<RinexObsHeader::RinexObsType, double> coeffList;
      
   }; // end class LinearCombination

   //@}

}  // end namespace gpstk

#endif // GPSTK_LINEAR_COMBINATION_HPP
