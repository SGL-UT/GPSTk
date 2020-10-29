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
/**
 * @file DiffCorrBase.hpp
 * General data and methods for cracking CDC and EDC packets from CNAV 
 * MT 34, 13, 14 OR CNAV-2 subframe 3 page 5. 
 *
 */

#ifndef SGLTK_DIFFCORRBASE_HPP
#define SGLTK_DIFFCORRBASE_HPP

#include <string>
#include <iostream>

#include "CommonTime.hpp"
#include "Exception.hpp"
#include "PackedNavBits.hpp"
#include "SatID.hpp"

namespace gpstk
{
   class DiffCorrBase
   {
   public:
      enum DCTYPE
      {
         dtUnknown,
         dtLNAV,
         dtCNAV,
         dtCNAV2
      };

         /// Default constructor
      DiffCorrBase();

         /**
          * @throw InvalidParameter
          */
      DiffCorrBase(const PackedNavBits& msg, 
                   const unsigned startBit);
        
         /// Destructor
      virtual ~DiffCorrBase() {}

      /**
       * Any PackedNavBits may be provided to the loadData()
       * method.  Only the following messages will be processed.
       *  - CNAV MT 34
       *  - CNAV MT 13
       *  - CNAV MT 14
       *  - CNAV-2 subframe 3, page 5
       * Any other messages will simply return with no action.
       * If any of the specified messages are provided, the 
       * CDC packet starting at the specified bit index
       * (zero-based) will be cracked and stored. 
       * @throw InvalidParameter if a specific message is provided but
       * the parsing of the message data is invalid.
       */
      virtual void loadData(const PackedNavBits& msg, 
                            const unsigned startBit);
  
      virtual bool isSameData(const DiffCorrBase& right) const;

         /**
          * @throw InvalidRequest
          */
      virtual void dump(std::ostream& s = std::cout) const;

      CommonTime topD;
      CommonTime tOD;
      SatID subjSv;
      SatID xmitSv;
      DCTYPE dcDataType; 
      bool dataLoadedFlag; 

   }; // end class DiffCorrBase

} // end namespace gpstk

#endif 

