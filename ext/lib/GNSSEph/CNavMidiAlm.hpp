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
 * @file CNavMidiAlm.hpp
 * Enables cracking of CNAV and CNAV-2 Midi Almanac packet data. 
 *
 */

#ifndef GPSTK_CNAVMIDIALM_HPP
#define GPSTK_CNAVMIDIALM_HPP

#include <string>
#include <iostream>
#include "CommonTime.hpp"
#include "OrbData.hpp"
#include "OrbDataSys.hpp"
#include "PackedNavBits.hpp"

namespace gpstk
{
   // This one inherits from OrbDataSys because it is shared between
   // CNAV and CNAV-2.  Therefore, neither OrbSysGpsC nor the CNAV-2
   // equivalent is correct. 
   class CNavMidiAlm : public OrbDataSys
   {
   public:
      enum MidiAlmType
      {
         matCNAV,
         matCNAV2,
         matUnknown
      };

         /// Default constructor
      CNavMidiAlm();

         /// Clone method
      virtual CNavMidiAlm* clone() const;

         /** pnb      - CNAV MT37 or CNAV-2 subframe 3, page 4. 
          * @throw InvalidParameter
          */
      CNavMidiAlm(const PackedNavBits& pnb);
  
         /**
          * @throw InvalidParameter
          */
      void loadData(const PackedNavBits& pnb);

      bool isSameData(const gpstk::OrbData* right) const;      
 
      virtual std::string getName() const
      {
         return "MIDI ALM";
      }

      virtual std::string getNameLong() const
      {
         return "GPS CNAV/CNAV-2 MIDI ALMANAC";
      }

         /** Output the contents of this orbit data to the given stream.
          * @throw InvalidRequest if the required data has not been stored.
          */
      virtual void dumpTerse(std::ostream& s = std::cout) const;

         /**
          * @throw InvalidRequest
          */
      virtual void dumpHeader(std::ostream& s = std::cout) const;

         /**
          * @throw InvalidRequest
          */
      virtual void dumpBody(std::ostream& s = std::cout) const;

      MidiAlmType almType; 
      CommonTime ctAlmEpoch;
      SatID  xmitSv;
      SatID  subjSv;
      double e;
      double di;
      double OMEGAdot;
      double sqrtA;
      double OMEGA0;
      double w;
      double M0;
      double af0;
      double af1;
      unsigned short L1HEALTH;
      unsigned short L2HEALTH;
      unsigned short L5HEALTH;

   private:
      std::string goodBad(const unsigned val, std::ostream& s) const;

   }; // end class CNavMidiAlm

} // end namespace gpstk

#endif 
