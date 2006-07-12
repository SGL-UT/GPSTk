#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/receiver/RinexObsRoller.cpp#1 $"

/**
 * @file LinearCombination.cpp
 * Interprets an expression of linear combinations of RINEX observables.
 * Class definitions.
 */

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

#include <iomanip>
#include <fstream>
#include <string>
#include <map>

#include "LinearCombination.hpp"

namespace gpstk
{
   
   LinearCombination::LinearCombination(const std::string& expr)
   {   
   }
   
   LinearCombination::~LinearCombination(void)
   {
   }

   EpochCombination LinearCombination::evaluate(const RinexObsData& rod)
   {
      EpochCombination result;

      RinexObsData::RinexPrnMap::const_iterator it;
      for (it = rod.obs.begin(); it!= rod.obs.end(); it++)
      {
         RinexObsData::RinexObsTypeMap otmap = it->second;
         RinexObsData::RinexObsTypeMap::const_iterator 
            itObs;
         
         std::map<RinexObsHeader::RinexObsType, double>::iterator itCoeff;
         double sum=0;
         
         for (itCoeff=coeffList.begin(); itCoeff!=coeffList.end(); itCoeff++)
         {
            bool valid= ((itObs=otmap.find(itCoeff->first))!=otmap.end());
            sum += itObs->second.data * itCoeff->second;
          }
          
         result[it->first] = sum;
         
   }


      return (result);
   }
   
   
} // namespace gpstk

