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
 * @file CNavReducedAlm.hpp
 * Enables cracking of CNAV and CNAV-2 Reduced Almanac packet data. 
 *
 */

#ifndef GPSTK_CNAVREDUCEDALM_HPP
#define GPSTK_CNAVREDUCEDALM_HPP

#include <string>
#include <iostream>
#include "CommonTime.hpp"
#include "PackedNavBits.hpp"

namespace gpstk
{
   class CNavReducedAlm
   {
   public:
      enum AlmType
      {
         atCNAV,
         atCNAV2
      };

         /// Default constructor
      CNavReducedAlm();

         /** AlmType  - Denotes CNAV or CNAV-2
          * ctAlm    - Epoch time of almanac (toa) as provided in full message.
          * pnb      - CNAV MT31, CNAV 12, or CNAV-2 subframe 3, page 3. 
          * startBit - Location within pnb where packet to be cracked starts.  
          *            This is a zero-based index.
          * @throw InvalidParameter
          */
      CNavReducedAlm(const AlmType almType, const CommonTime& ctAlm, 
                     const PackedNavBits& pnb, const unsigned int startBit);
  
         /**
          * @throw InvalidParameter
          */
      void loadData(const AlmType, const CommonTime& ctAlm, const PackedNavBits& pnb, const unsigned int startBit);

      bool isSameData(const CNavReducedAlm& right) const;      

         /** Output the contents of this orbit data to the given stream.
          * @throw InvalidRequest if the required data has not been stored.
          */
      virtual void dumpTerse(std::ostream& s = std::cout) const;

         /**
          * @throw InvalidRequest
          */
      static void dumpHeader(std::ostream& s = std::cout);

         /**
          * @throw InvalidRequest
          */
      virtual void dumpBody(std::ostream& s = std::cout) const;

      CommonTime ctAlmEpoch;
      SatID  subjSv;
      double deltaA;
      double A;
      double OMEGA0;
      double Psi0;
      unsigned short L1HEALTH;
      unsigned short L2HEALTH;
      unsigned short L5HEALTH;
      bool dataLoadedFlag;

   protected:
      static const double A_ref;

   }; // end class CNavReducedAlm

} // end namespace gpstk

#endif 
