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

/**
 * @file RinexObsID.cpp
 * gpstk::RinexObsID - Identifies types of observations
 */

#include "RinexObsID.hpp"
#include "RinexSatID.hpp"
#include "StringUtils.hpp"
#include "Rinex3ObsHeader.hpp"

namespace gpstk
{
      // Mappings between code, carrier, type and characters.
      // The following definitions really should only describe the
      // items that are in the Rinex 3 specification. If an
      // application needs additional ObsID types to be able to be
      // translated to/from Rinex3, the additional types must be added
      // by the application.
   std::map< char, gpstk::ObservationType> RinexObsID::char2ot {
      { ' ', ObservationType::Unknown },
      { '*', ObservationType::Any },
      { 'C', ObservationType::Range },
      { 'L', ObservationType::Phase },
      { 'D', ObservationType::Doppler },
      { 'I', ObservationType::Iono },
      { 'S', ObservationType::SNR },
      { 'X', ObservationType::Channel },
      { '-', ObservationType::Undefined }
   };

   std::map< char, CarrierBand> RinexObsID::char2cb {
      { ' ', CarrierBand::Unknown },
      { '*', CarrierBand::Any },
      { '1', CarrierBand::L1 },
      { '2', CarrierBand::L2 },
      { '3', CarrierBand::G3 },
      { '4', CarrierBand::G1a },
      { '5', CarrierBand::L5 },
      { '6', CarrierBand::E6 },
      { '7', CarrierBand::E5b },
      { '8', CarrierBand::E5ab },
      { '9', CarrierBand::I9 },
      { '-', CarrierBand::Undefined }
   };

   std::map< char, TrackingCode> RinexObsID::char2tc {
      { ' ', TrackingCode::Unknown },
      { '*', TrackingCode::Any },
      { 'C', TrackingCode::CA },
      { 'P', TrackingCode::P },
      { 'W', TrackingCode::W },
      { 'Y', TrackingCode::Y },
      { 'M', TrackingCode::M },
      { 'N', TrackingCode::N },
      { 'D', TrackingCode::D },
      { 'S', TrackingCode::C2M },
      { 'L', TrackingCode::C2L },
      { 'X', TrackingCode::C2LM },
      { 'I', TrackingCode::I5 },
      { 'Q', TrackingCode::Q5 },
      { 'A', TrackingCode::A },
      { 'B', TrackingCode::B },
      { 'Z', TrackingCode::ABC },
      { 'E', TrackingCode::JE6 },
      { '-', TrackingCode::Undefined }
   };

   std::map< gpstk::ObservationType, char > RinexObsID::ot2char {
      { ObservationType::Unknown,    ' ' },
      { ObservationType::Any,        '*' },
      { ObservationType::Range,      'C' },
      { ObservationType::Phase,      'L' },
      { ObservationType::Doppler,    'D' },
      { ObservationType::SNR,        'S' },
      { ObservationType::Channel,    'X' },
      { ObservationType::Iono,       'I' },
      { ObservationType::Undefined,  '-' },
   };

   std::map< CarrierBand, char > RinexObsID::cb2char {
      { CarrierBand::L1,         '1' },
      { CarrierBand::L2,         '2' },
      { CarrierBand::L5,         '5' },
      { CarrierBand::G1,         '1' },
      { CarrierBand::G1a,        '4' },
      { CarrierBand::G2a,        '6' },
      { CarrierBand::G2,         '2' },
      { CarrierBand::G3,         '3' },
      { CarrierBand::E6,         '6' },
      { CarrierBand::E5b,        '7' },
      { CarrierBand::E5ab,       '8' },
      { CarrierBand::B1,         '2' },
      { CarrierBand::B3,         '6' },
      { CarrierBand::B2,         '7' },
      { CarrierBand::I9,         '9' },
      { CarrierBand::Unknown,    ' ' },
      { CarrierBand::Any,        '*' },
      { CarrierBand::Undefined,  '-' }
   };

   std::map< TrackingCode, char> RinexObsID::tc2char {
      { TrackingCode::CA,          'C' },
      { TrackingCode::N,           'N' },
      { TrackingCode::I5,          'I' },
      { TrackingCode::G1D,         'S' },
      { TrackingCode::G1X,         'X' },
      { TrackingCode::G1P,         'L' },
      { TrackingCode::C2LM,        'X' },
      { TrackingCode::C2L,         'L' },
      { TrackingCode::C2M,         'S' },
      { TrackingCode::IQ5,         'X' },
      { TrackingCode::M,           'M' },
      { TrackingCode::P,           'P' },
      { TrackingCode::Q5,          'Q' },
      { TrackingCode::D,           'D' },
      { TrackingCode::Y,           'Y' },
      { TrackingCode::W,           'W' },
      { TrackingCode::L1OCD,       'A' },
      { TrackingCode::L1OCP,       'B' },
      { TrackingCode::L1OC,        'X' },
      { TrackingCode::L2CSIOCp,    'X' },
      { TrackingCode::L2CSI,       'A' },
      { TrackingCode::L2OCP,       'B' },
      { TrackingCode::IR3,         'I' },
      { TrackingCode::IQR3,        'X' },
      { TrackingCode::QR3,         'Q' },
      { TrackingCode::GP,          'P' },
      { TrackingCode::GCA,         'C' },
      { TrackingCode::A,           'A' },
      { TrackingCode::ABC,         'Z' },
      { TrackingCode::B,           'B' },
      { TrackingCode::BC,          'X' },
      { TrackingCode::C,           'C' },
      { TrackingCode::IE5,         'I' },
      { TrackingCode::IQE5,        'X' },
      { TrackingCode::QE5,         'Q' },
      { TrackingCode::IE5a,        'I' },
      { TrackingCode::IQE5a,       'X' },
      { TrackingCode::QE5a,        'Q' },
      { TrackingCode::IE5b,        'I' },
      { TrackingCode::IQE5b,       'X' },
      { TrackingCode::QE5b,        'Q' },
      { TrackingCode::A6,          'A' },
      { TrackingCode::ABC6,        'Z' },
      { TrackingCode::B6,          'B' },
      { TrackingCode::BC6,         'X' },
      { TrackingCode::C6,          'C' },
      { TrackingCode::SCA,         'C' },
      { TrackingCode::SI5,         'I' },
      { TrackingCode::SIQ5,        'X' },
      { TrackingCode::SQ5,         'Q' },
      { TrackingCode::JCA,         'C' },
      { TrackingCode::JD1,         'S' },
      { TrackingCode::JX1,         'X' },
      { TrackingCode::JP1,         'L' },
      { TrackingCode::JZ1,         'Z' },
      { TrackingCode::JL2,         'L' },
      { TrackingCode::JM2,         'S' },
      { TrackingCode::JX2,         'X' },
      { TrackingCode::JI5,         'I' },
      { TrackingCode::JIQ5,        'X' },
      { TrackingCode::JQ5,         'Q' },
      { TrackingCode::JI5S,        'D' },
      { TrackingCode::JIQ5S,       'Z' },
      { TrackingCode::JQ5S,        'P' },
      { TrackingCode::JI6,         'S' },
      { TrackingCode::JIQ6,        'X' },
      { TrackingCode::JQ6,         'L' },
      { TrackingCode::JDE6,        'Z' },
      { TrackingCode::JD6,         'S' },
      { TrackingCode::JE6,         'E' },
      { TrackingCode::CIQ1,        'X' },
      { TrackingCode::CA1,         'A' },
      { TrackingCode::CCD1,        'D' },
      { TrackingCode::CCDP1,       'X' },
      { TrackingCode::CCP1,        'P' },
      { TrackingCode::CI1,         'I' },
      { TrackingCode::CQ1,         'Q' },
      { TrackingCode::CI2ab,       'D' },
      { TrackingCode::CIQ2ab,      'X' },
      { TrackingCode::CQ2ab,       'P' },
      { TrackingCode::CIQ7,        'X' },
      { TrackingCode::CI2a,        'D' },
      { TrackingCode::CIQ2a,       'X' },
      { TrackingCode::CQ2a,        'P' },
      { TrackingCode::CI2b,        'D' },
      { TrackingCode::CIQ2b,       'Z' },
      { TrackingCode::CQ2b,        'P' },
      { TrackingCode::CI7,         'I' },
      { TrackingCode::CQ7,         'Q' },
      { TrackingCode::CIQ6,        'X' },
      { TrackingCode::CI6,         'I' },
      { TrackingCode::CQ6,         'Q' },
      { TrackingCode::CodelessC,   'N' },
      { TrackingCode::CIQ3A,       'A' },
      { TrackingCode::IB5,         'B' },
      { TrackingCode::IX5,         'X' },
      { TrackingCode::IC5,         'C' },
      { TrackingCode::IB9,         'B' },
      { TrackingCode::IX9,         'X' },
      { TrackingCode::IC9,         'C' },
      { TrackingCode::IA5,         'A' },
      { TrackingCode::IA9,         'A' },
      { TrackingCode::Unknown,     ' ' },
      { TrackingCode::Any,         '*' },
      { TrackingCode::Undefined,   '-' }
   };

   // map of valid RINEX tracking codes, systems and frequency
   std::map<char, std::map<char, std::string> >
   RinexObsID::validRinexTrackingCodes {
         
      { 'G', {
            { '1', "PYWLMCSXN* " },  // except no C1N
            { '2', "PYWLMCSXDN* " }, // except no C2N
            { '5', "IQX* " } } },

      { 'R', {
            { '1', "PC* " },
            { '2', "PC* " },
            { '3', "IQX* " },
            { '4', "ABX* " },
            { '6', "ABX* " } } },

      { 'E', {
            { '1', "ABCXZ* " },
            { '5', "IQX* " },
            { '6', "ABCXZ* " },
            { '7', "IQX* " },
            { '8', "IQX* " } } },

      { 'S', {
            { '1', "C* " },
            { '5', "IQX* " } } },

         /** @note 3.02 uses carrier "1" for "B1-2", every other RINEX
          * version uses carrier "2", so we have to leave IQ in C1 */
      { 'C', {
            { '1', "PAIQXDN* " },
            { '2', "IQX* " },
            { '5', "PXD* " },
            { '6', "AIQX* " },
            { '7', "ZPIQXD* " },
            { '8', "PXD* " } } },

      { 'J', {
            { '1', "CSLXZ* " },
            { '2', "SLX* " },
            { '5', "IQDPXZ* " },
            { '6', "ESLXZ* " } } },

         // I1 is *only* for channel number
      { 'I', {
            { '1', "* " },
            { '5', "ABCX* " },
            { '9', "ABCX* " } } }
   };

   // string containing the system characters for all valid RINEX systems.
   std::string RinexObsID::validRinexSystems("GRESCJI");

   // maps between 1-char and 3-char system id
   std::map<std::string, std::string> RinexObsID::map1to3sys {
      { "G", "GPS" },
      { "R", "GLO" },
      { "E", "GAL" },
      { "S", "GEO" },
      { "C", "BDS" },
      { "J", "QZS" },
      { "I", "IRN" }
   };

   std::map<std::string, std::string> RinexObsID::map3to1sys {
      { "GPS", "G" },
      { "GLO", "R" },
      { "GAL", "E" },
      { "GEO", "S" },
      { "BDS", "C" },
      { "QZS", "J" },
      { "IRN", "I" }
   };

   // string containing the frequency digits for all valid RINEX systems.
   std::string RinexObsID::validRinexFrequencies("123456789");

   /// Construct this object from the string specifier
   RinexObsID::RinexObsID(const std::string& strID, double version)
         : rinexVersion(version)
   {
      std::string modStrID(strID);
      type = ObservationType::Unknown;
      band = CarrierBand::Unknown;
      code = TrackingCode::Unknown;
      int i = modStrID.length() - 3;
      if ( i < 0 || i > 1)
      {
         InvalidParameter e("identifier must be 3 or 4 characters long");
         GPSTK_THROW(e);
      }
      else if (i == 0)
      {
         modStrID = std::string("G") + modStrID; // GPS is default
      }

         // make an initial guess
      const auto& tc = char2tc.find(modStrID[3]);
      const auto& cb = char2cb.find(modStrID[2]);
      const auto& ot = char2ot.find(modStrID[1]);
      if (tc != char2tc.end())
         code = tc->second;
      if (cb != char2cb.end())
         band = cb->second;
      if (ot != char2ot.end())
         type = ot->second;

      std::string rincode(modStrID.substr(2));
         // do the observation type which is mapped 1:1
      switch (modStrID[1])
      {
         case ' ': // Unknown
            type = ObservationType::Unknown;
            break;
         case '*': // Any
            type = ObservationType::Any;
            break;
         case 'C': // Range
            type = ObservationType::Range;
            break;
         case 'L': // Phase
            type = ObservationType::Phase;
            break;
         case 'D': // Doppler
            type = ObservationType::Doppler;
            break;
         case 'S': // SNR
            type = ObservationType::SNR;
            break;
         case 'X': // Channel
            type = ObservationType::Channel;
            if ((modStrID[2] != '1') || (modStrID[3] != ' '))
            {
               InvalidParameter e("Invalid channel number pseudo-obs ID " +
                                  strID);
               GPSTK_THROW(e);
            }
            code = TrackingCode::Undefined;
               /** @bug Not convinced this is an appropriate value, but
                * it's what has been used in the past. */
            band = CarrierBand::L1;
            break;
         case 'I': // Iono
            type = ObservationType::Iono;
            if ((modStrID[2] < '1') || (modStrID[2] > '9') ||
                (modStrID[3] != ' '))
            {
               InvalidParameter e("Invalid ionospheric delay pseudo-obs ID " +
                                  strID);
               GPSTK_THROW(e);
            }
            code = TrackingCode::Undefined;
            band = char2cb[modStrID[2]];
            break;
         case '-': // Undefined
            type = ObservationType::Undefined;
            break;
         default:
            type = ObservationType::Unknown;
            break;
      }
         // group first by rinex system ID
      switch (modStrID[0])
      {
         case 'C': // BeiDou
            if (rincode == "2I")
            {
               band = CarrierBand::B1;
               code = TrackingCode::CI1;
            }
            else if (rincode == "2Q")
            {
               band = CarrierBand::B1;
               code = TrackingCode::CQ1;
            }
            else if (rincode == "2X")
            {
               band = CarrierBand::B1;
               code = TrackingCode::CIQ1;
            }
            else if (rincode == "1D")
            {
               band = CarrierBand::L1;
               code = TrackingCode::CCD1;
            }
            else if (rincode == "1P")
            {
               band = CarrierBand::L1;
               code = TrackingCode::CCP1;
            }
            else if (rincode == "1X")
            {
               if (fabs(rinexVersion - 3.02) < 0.005)
               {
                  band = CarrierBand::B1;
                  code = TrackingCode::CIQ1;
               }
               else
               {
                  band = CarrierBand::L1;
                  code = TrackingCode::CCDP1;
               }
            }
            else if (rincode == "1A")
            {
               band = CarrierBand::L1;
               code = TrackingCode::CA1;
            }
            else if (rincode == "1N")
            {
               band = CarrierBand::L1;
               code = TrackingCode::CodelessC;
            }
            else if (rincode == "1I")
            {
               band = CarrierBand::B1;
               code = TrackingCode::CI1;
            }
            else if (rincode == "1Q")
            {
               band = CarrierBand::B1;
               code = TrackingCode::CQ1;
            }
            else if (rincode == "5D")
            {
               band = CarrierBand::L5;
               code = TrackingCode::CI2a;
            }
            else if (rincode == "5P")
            {
               band = CarrierBand::L5;
               code = TrackingCode::CQ2a;
            }
            else if (rincode == "5X")
            {
               band = CarrierBand::L5;
               code = TrackingCode::CIQ2a;
            }
            else if (rincode == "7I")
            {
               band = CarrierBand::B2;
               code = TrackingCode::CI7;
            }
            else if (rincode == "7Q")
            {
               band = CarrierBand::B2;
               code = TrackingCode::CQ7;
            }
            else if (rincode == "7X")
            {
               band = CarrierBand::B2;
               code = TrackingCode::CIQ7;
            }
            else if (rincode == "7D")
            {
               band = CarrierBand::B2;
               code = TrackingCode::CI2b;
            }
            else if (rincode == "7P")
            {
               band = CarrierBand::B2;
               code = TrackingCode::CQ2b;
            }
            else if (rincode == "7Z")
            {
               band = CarrierBand::B2;
               code = TrackingCode::CIQ2b;
            }
            else if (rincode == "8D")
            {
               band = CarrierBand::E5ab;
               code = TrackingCode::CI2ab;
            }
            else if (rincode == "8P")
            {
               band = CarrierBand::E5ab;
               code = TrackingCode::CQ2ab;
            }
            else if (rincode == "8X")
            {
               band = CarrierBand::E5ab;
               code = TrackingCode::CIQ2ab;
            }
            else if (rincode == "6I")
            {
               band = CarrierBand::B3;
               code = TrackingCode::CI6;
            }
            else if (rincode == "6Q")
            {
               band = CarrierBand::B3;
               code = TrackingCode::CQ6;
            }
            else if (rincode == "6X")
            {
               band = CarrierBand::B3;
               code = TrackingCode::CIQ6;
            }
            else if (rincode == "6A")
            {
               band = CarrierBand::B3;
               code = TrackingCode::CIQ3A;
            }
            break;
         case 'E': // Galileo
            if (rincode == "1A")
            {
               band = CarrierBand::L1;
               code = TrackingCode::A;
            }
            else if (rincode == "1B")
            {
               band = CarrierBand::L1;
               code = TrackingCode::B;
            }
            else if (rincode == "1C")
            {
               band = CarrierBand::L1;
               code = TrackingCode::C;
            }
            else if (rincode == "1X")
            {
               band = CarrierBand::L1;
               code = TrackingCode::BC;
            }
            else if (rincode == "1Z")
            {
               band = CarrierBand::L1;
               code = TrackingCode::ABC;
            }
            else if (rincode == "5I")
            {
               band = CarrierBand::L5;
               code = TrackingCode::IE5a;
            }
            else if (rincode == "5Q")
            {
               band = CarrierBand::L5;
               code = TrackingCode::QE5a;
            }
            else if (rincode == "5X")
            {
               band = CarrierBand::L5;
               code = TrackingCode::IQE5a;
            }
            else if (rincode == "7I")
            {
               band = CarrierBand::E5b;
               code = TrackingCode::IE5b;
            }
            else if (rincode == "7Q")
            {
               band = CarrierBand::E5b;
               code = TrackingCode::QE5b;
            }
            else if (rincode == "7X")
            {
               band = CarrierBand::E5b;
               code = TrackingCode::IQE5b;
            }
            else if (rincode == "8I")
            {
               band = CarrierBand::E5ab;
               code = TrackingCode::IE5;
            }
            else if (rincode == "8Q")
            {
               band = CarrierBand::E5ab;
               code = TrackingCode::QE5;
            }
            else if (rincode == "8X")
            {
               band = CarrierBand::E5ab;
               code = TrackingCode::IQE5;
            }
            else if (rincode == "6A")
            {
               band = CarrierBand::E6;
               code = TrackingCode::A6;
            }
            else if (rincode == "6B")
            {
               band = CarrierBand::E6;
               code = TrackingCode::B6;
            }
            else if (rincode == "6C")
            {
               band = CarrierBand::E6;
               code = TrackingCode::C6;
            }
            else if (rincode == "6X")
            {
               band = CarrierBand::E6;
               code = TrackingCode::BC6;
            }
            else if (rincode == "6Z")
            {
               band = CarrierBand::E6;
               code = TrackingCode::ABC6;
            }
            break;
         case 'R': // GLONASS
            if (rincode == "1C")
            {
               band = CarrierBand::G1;
               code = TrackingCode::GCA;
            }
            else if (rincode == "1P")
            {
               band = CarrierBand::G1;
               code = TrackingCode::GP;
            }
            else if (rincode == "4A")
            {
               band = CarrierBand::G1a;
               code = TrackingCode::L1OCD;
            }
            else if (rincode == "4B")
            {
               band = CarrierBand::G1a;
               code = TrackingCode::L1OCP;
            }
            else if (rincode == "4X")
            {
               band = CarrierBand::G1a;
               code = TrackingCode::L1OC;
            }
            else if (rincode == "2C")
            {
               band = CarrierBand::G2;
               code = TrackingCode::GCA;
            }
            else if (rincode == "2P")
            {
               band = CarrierBand::G2;
               code = TrackingCode::GP;
            }
            else if (rincode == "6A")
            {
               band = CarrierBand::G2a;
               code = TrackingCode::L2CSI;
            }
            else if (rincode == "6B")
            {
               band = CarrierBand::G2a;
               code = TrackingCode::L2OCP;
            }
            else if (rincode == "6X")
            {
               band = CarrierBand::G2a;
               code = TrackingCode::L2CSIOCp;
            }
            else if (rincode == "3I")
            {
               band = CarrierBand::G3;
               code = TrackingCode::IR3;
            }
            else if (rincode == "3Q")
            {
               band = CarrierBand::G3;
               code = TrackingCode::QR3;
            }
            else if (rincode == "3X")
            {
               band = CarrierBand::G3;
               code = TrackingCode::IQR3;
            }
            break;
         case 'G': // GPS
            if (rincode == "1C")
            {
               band = CarrierBand::L1;
               code = TrackingCode::CA;
            }
            else if (rincode == "1S")
            {
               band = CarrierBand::L1;
               code = TrackingCode::G1D;
            }
            else if (rincode == "1L")
            {
               band = CarrierBand::L1;
               code = TrackingCode::G1P;
            }
            else if (rincode == "1X")
            {
               band = CarrierBand::L1;
               code = TrackingCode::G1X;
            }
            else if (rincode == "1P")
            {
               band = CarrierBand::L1;
               code = TrackingCode::P;
            }
            else if (rincode == "1W")
            {
               band = CarrierBand::L1;
               code = TrackingCode::W;
            }
            else if (rincode == "1Y")
            {
               band = CarrierBand::L1;
               code = TrackingCode::Y;
            }
            else if (rincode == "1M")
            {
               band = CarrierBand::L1;
               code = TrackingCode::M;
            }
            else if (rincode == "1N")
            {
               band = CarrierBand::L1;
               code = TrackingCode::N;
            }
            else if (rincode == "2C")
            {
               band = CarrierBand::L2;
               code = TrackingCode::CA;
            }
            else if (rincode == "2D")
            {
               band = CarrierBand::L2;
               code = TrackingCode::D;
            }
            else if (rincode == "2S")
            {
               band = CarrierBand::L2;
               code = TrackingCode::C2M;
            }
            else if (rincode == "2L")
            {
               band = CarrierBand::L2;
               code = TrackingCode::C2L;
            }
            else if (rincode == "2X")
            {
               band = CarrierBand::L2;
               code = TrackingCode::C2LM;
            }
            else if (rincode == "2P")
            {
               band = CarrierBand::L2;
               code = TrackingCode::P;
            }
            else if (rincode == "2W")
            {
               band = CarrierBand::L2;
               code = TrackingCode::W;
            }
            else if (rincode == "2Y")
            {
               band = CarrierBand::L2;
               code = TrackingCode::Y;
            }
            else if (rincode == "2M")
            {
               band = CarrierBand::L2;
               code = TrackingCode::M;
            }
            else if (rincode == "2N")
            {
               band = CarrierBand::L2;
               code = TrackingCode::N;
            }
            else if (rincode == "5I")
            {
               band = CarrierBand::L5;
               code = TrackingCode::I5;
            }
            else if (rincode == "5Q")
            {
               band = CarrierBand::L5;
               code = TrackingCode::Q5;
            }
            else if (rincode == "5X")
            {
               band = CarrierBand::L5;
               code = TrackingCode::IQ5;
            }
            break;
         case 'I': // NavIC
            if (rincode == "5A")
            {
               band = CarrierBand::L5;
               code = TrackingCode::IA5;
            }
            else if (rincode == "5B")
            {
               band = CarrierBand::L5;
               code = TrackingCode::IB5;
            }
            else if (rincode == "5C")
            {
               band = CarrierBand::L5;
               code = TrackingCode::IC5;
            }
            else if (rincode == "5X")
            {
               band = CarrierBand::L5;
               code = TrackingCode::IX5;
            }
            else if (rincode == "9A")
            {
               band = CarrierBand::I9;
               code = TrackingCode::IA9;
            }
            else if (rincode == "9B")
            {
               band = CarrierBand::I9;
               code = TrackingCode::IB9;
            }
            else if (rincode == "9C")
            {
               band = CarrierBand::I9;
               code = TrackingCode::IC9;
            }
            else if (rincode == "9X")
            {
               band = CarrierBand::I9;
               code = TrackingCode::IX9;
            }
            break;
         case 'J': // QZSS
            if (rincode == "1C")
            {
               band = CarrierBand::L1;
               code = TrackingCode::JCA;
            }
            else if (rincode == "1L")
            {
               band = CarrierBand::L1;
               code = TrackingCode::JP1;
            }
            else if (rincode == "1S")
            {
               band = CarrierBand::L1;
               code = TrackingCode::JD1;
            }
            else if (rincode == "1X")
            {
               band = CarrierBand::L1;
               code = TrackingCode::JX1;
            }
            else if (rincode == "1Z")
            {
               band = CarrierBand::L1;
               code = TrackingCode::JZ1;
            }
            else if (rincode == "2S")
            {
               band = CarrierBand::L2;
               code = TrackingCode::JM2;
            }
            else if (rincode == "2L")
            {
               band = CarrierBand::L2;
               code = TrackingCode::JL2;
            }
            else if (rincode == "2X")
            {
               band = CarrierBand::L2;
               code = TrackingCode::JX2;
            }
            else if (rincode == "5I")
            {
               band = CarrierBand::L5;
               code = TrackingCode::JI5;
            }
            else if (rincode == "5Q")
            {
               band = CarrierBand::L5;
               code = TrackingCode::JQ5;
            }
            else if (rincode == "5X")
            {
               band = CarrierBand::L5;
               code = TrackingCode::JIQ5;
            }
            else if (rincode == "5D")
            {
               band = CarrierBand::L5;
               code = TrackingCode::JI5S;
            }
            else if (rincode == "5P")
            {
               band = CarrierBand::L5;
               code = TrackingCode::JQ5S;
            }
            else if (rincode == "5Z")
            {
               band = CarrierBand::L5;
               code = TrackingCode::JIQ5S;
            }
            else if (rincode == "6S")
            {
               band = CarrierBand::E6;
               code = TrackingCode::JI6;
            }
            else if (rincode == "6S")
            {
               band = CarrierBand::E6;
               code = TrackingCode::JD6;
            }
            else if (rincode == "6L")
            {
               band = CarrierBand::E6;
               code = TrackingCode::JQ6;
            }
            else if (rincode == "6X")
            {
               band = CarrierBand::E6;
               code = TrackingCode::JIQ6;
            }
            else if (rincode == "6E")
            {
               band = CarrierBand::E6;
               code = TrackingCode::JE6;
            }
            else if (rincode == "6Z")
            {
               band = CarrierBand::E6;
               code = TrackingCode::JDE6;
            }
            break;
         case 'S': // SBAS
            if (rincode == "1C")
            {
               band = CarrierBand::L1;
               code = TrackingCode::SCA;
            }
            else if (rincode == "5I")
            {
               band = CarrierBand::L5;
               code = TrackingCode::SI5;
            }
            else if (rincode == "5Q")
            {
               band = CarrierBand::L5;
               code = TrackingCode::SQ5;
            }
            else if (rincode == "5X")
            {
               band = CarrierBand::L5;
               code = TrackingCode::SIQ5;
            }
            break;
      }

         // Commenting this out because it breaks support of codes
         // that would have been supported in prior releases by ObsID.
         // if (!isValidRinexObsID(strID))
         // {
         //    InvalidParameter ip(strID + " is not a valid RinexObsID");
         //    GPSTK_THROW(ip);
         // }
   }

   RinexObsID::RinexObsID(const RinexObsType& rot)
         : ObsID()
   {
         // Note that the choice of tracking code for L1, L2, S1, S2
         // are arbitrary since they are ambiguous in the rinex 2
         // specifications
         // L1 -> L1P; P1 -> C1P; C1 -> C1C; S1 -> S1P; D1 -> D1P
      if (rot == RinexObsHeader::L1)
      {
         type=ObservationType::Phase;
         band=CarrierBand::L1;
         code=TrackingCode::P;
      }
      else if (rot == RinexObsHeader::P1)
      {
         type=ObservationType::Range;
         band=CarrierBand::L1;
         code=TrackingCode::P;
      }
      else if (rot == RinexObsHeader::C1)
      {
         type=ObservationType::Range;
         band=CarrierBand::L1;
         code=TrackingCode::CA;
      }
      else if (rot == RinexObsHeader::S1)
      {
         type=ObservationType::SNR;
         band=CarrierBand::L1;
         code=TrackingCode::P;
      }
      else if (rot == RinexObsHeader::D1)
      {
         type=ObservationType::Doppler;
         band=CarrierBand::L1;
         code=TrackingCode::P;
      }
         // L2 -> L2P; P2 -> C2P; C2 -> C2X; S2 -> S2P; D2 -> D2P
      else if (rot == RinexObsHeader::L2)
      {
         type=ObservationType::Phase;
         band=CarrierBand::L2;
         code=TrackingCode::P;
      }
      else if (rot == RinexObsHeader::P2)
      {
         type=ObservationType::Range;
         band=CarrierBand::L2;
         code=TrackingCode::P;
      }
      else if (rot == RinexObsHeader::C2)
      {
         type=ObservationType::Range;
         band=CarrierBand::L2;
         code=TrackingCode::C2LM;
      }
      else if (rot == RinexObsHeader::S2)
      {
         type=ObservationType::SNR;
         band=CarrierBand::L2;
         code=TrackingCode::P;
      }
      else if (rot == RinexObsHeader::D2)
      {
         type=ObservationType::Doppler;
         band=CarrierBand::L2;
         code=TrackingCode::P;
      }
   }

   // Represent this object using the Rinex3 notation
   std::string RinexObsID::asString(double version) const
   {
      char buff[4];

      buff[0] = ot2char[type];
      buff[1] = cb2char[band];
      buff[2] = tc2char[code];
      if ((fabs(version - 3.02) < 0.005) && (band == CarrierBand::B1) &&
          ((code == TrackingCode::CI1) || (code == TrackingCode::CQ1) ||
           (code == TrackingCode::CIQ1)))
      {
            // kludge for RINEX 3.02 BDS codes
         buff[1] = '1';
      }
         // special cases.
      if (type == ObservationType::Iono)
      {
         buff[2] = ' ';
      }
      else if (type == ObservationType::Channel)
      {
         buff[1] = '1';
         buff[2] = ' ';
      }
      buff[3] = 0;
      return std::string(buff);
   }

   namespace StringUtils
   {
      // convert this object to a string representation
      std::string asString(const RinexObsID& p)
      {
         return p.asString();
      }
   }

   // RINEX 3.03 document, section 5.1
   // GPS
   //       L1   C,S,L,X,P,W,Y,M,N(but not C1N)       G 1 CSLXPWYMN (but not C1N)
   //       L2   C,D,S,L,X,P,W,Y,M,N(but not C2N)     G 2 CDSLXPWYMN (but not C1N)
   //       L5   I,Q,X                                G 5 IQX
   // GLO
   //       G1   C,P                                  R 1 CP
   //       G2   C,P                                  R 2 CP
   // GAL
   //       E1   A,B,C,X,Z                            E 1 ABCXZ
   //       E5a  I,Q,X                                E 5 IQX
   //       E5b  I,Q,X                                E 7 IQX
   //       E5ab I,Q,X                                E 8 IQX
   //       E6   A,B,C,X,Z                            E 6 ABCXZ
   // SBAS
   //       L1   C                                    S 1 C
   //       L5   I,Q,X                                S 5 IQX
   // BDS
   //       B1   I,Q,X                                C 1 IQX
   //       B2   I,Q,X                                C 7 IQX
   //       B3   I,Q,X                                C 6 IQX
   // QZSS
   //       L1   C,S,L,X,Z                            J 1 CSLXZ
   //       L2   S,L,X                                J 2 SLX
   //       L5   I,Q,X                                J 5 IQX
   //       L6   S,L,X                                J 6 SLX
   // IRNSS
   //       L5   A,B,C,X                              I 5 ABCX
   //       L9   A,B,C,X                              I 9 ABCX

   // Determine if the given ObsID is valid. If the input string is 3
   // characters long, the system is assumed to be GPS. If this string is 4
   // characters long, the first character is the system designator as
   // described in the Rinex 3 specification.
   bool isValidRinexObsID(const std::string& strID)
   {
      int i(static_cast<int>(strID.length())-3);
      if(i < 0 || i > 1)
      {
         return false;
      }

      char sys;
      std::string id;

      if(i == 1)
      {
         sys = strID[0];
         id = strID.substr(1);
         return isValidRinexObsID(id,sys);
      }

      // test all RINEX systems
      std::string syss(RinexObsID::validRinexSystems);
      for(size_t j=0; j<syss.size(); j++)
      {
         if(isValidRinexObsID(strID,syss[j]))
         {
            return true;
         }
      }

      return false;
   }

   // Determine if the given ObsID is valid, for the given system
   bool isValidRinexObsID(const std::string& strID, const char sys)
   {
      if(strID.length() != 3)
      {
         return false;
      }
      char ot(strID[0]);
      char cb(strID[1]);
      char tc(strID[2]);
      const std::string &codes(RinexObsID::validRinexTrackingCodes[sys][cb]);
      if(ot == ' ' || ot == '-')
      {
         return false;
      }
      if (RinexObsID::char2ot.find(ot) == RinexObsID::char2ot.end())
      {
         return false;
      }
      if(codes.find(std::string(1,tc)) == std::string::npos)
      {
         return false;
      }
         // special cases for iono and channel num
      if (ot == 'I' && ((tc != ' ') || (cb < '1') || (cb > '9')))
      {
         return false;
      }
      if (ot == 'X' && ((tc != ' ') || (cb != '1')))
      {
         return false;
      }
      if (((codes == "* ") || (codes == " *")) && (ot == 'I'))
      {
            // channel num must always be "band" 1, but if the system
            // doesn't have any actual data on "1" band, we don't want
            // to accidentally say that we can get iono delay data for
            // a band that isn't valid for the system.
         return false;
      }
      if(sys == 'G' && ot == 'C' && tc == 'N')           // the one exception
      {
         return false;
      }

      return true;
   }

   std::ostream& RinexObsID::dumpCheck(std::ostream& s)
   {
      try {
         const std::string types("CLDS");
         std::map<char,std::string>::const_iterator it;

         for(size_t i=0; i<RinexObsID::validRinexSystems.size(); i++) {
            char csys = RinexObsID::validRinexSystems[i];
            std::string sys = RinexObsID::validRinexSystems.substr(i,1);
            RinexSatID sat(sys);
            std::string system(sat.systemString());

            s << "System " << sys << " = " << system << ", frequencies ";
            for(it = RinexObsID::validRinexTrackingCodes[sys[0]].begin();
               it != RinexObsID::validRinexTrackingCodes[sys[0]].end(); ++it)
               s << it->first;
            s << std::endl;

            for(it = RinexObsID::validRinexTrackingCodes[sys[0]].begin();
               it != RinexObsID::validRinexTrackingCodes[sys[0]].end(); ++it)
            {
               s << "   " << system << "(" << sys << "), freq " << it->first
                  << ", codes '" << it->second << "'" << std::endl;
               std::string codes(it->second), str;
               for(size_t j=0; j<codes.size(); ++j) {
                  std::ostringstream oss1;
                  for(size_t k=0; k<types.size(); ++k) {
                     str = std::string(1,types[k]) + std::string(1,it->first)
                           + std::string(1,codes[j]);
                     std::ostringstream oss;
                     if(!isValidRinexObsID(str,csys))
                        oss << str << " " << "-INVALID-";
                     else
                     {
                        RinexObsID robsid(sys+str,
                                          Rinex3ObsBase::currentVersion);
                        oss << str << " " << robsid;
                     }
                     oss1 << " " << StringUtils::leftJustify(oss.str(),34);
                  }
                  s << StringUtils::stripTrailing(oss1.str()) << std::endl;
               }
            }
         }
      }
      catch(Exception& e) {
         s << "Exception: " << e.what() << std::endl;
         GPSTK_RETHROW(e);
      }

      return s;
   }

   bool RinexObsID ::
   equalIndex(const RinexObsID& right)
      const
   {
      if (type != right.type)
         return false;
      if (type == ObservationType::Iono)
      {
            // only check band for ionospheric delay.
         return band == right.band;
      }
      if (type == ObservationType::Channel)
      {
            // There's only one channel type pseudo-observable
         return true;
      }
         // use the default for everything else
      return operator==(right);
   }

      // This is used to register a new RinexObsID & Rinex 3
      // identifier.  The syntax for the Rinex 3 identifier is the
      // same as for the RinexObsID constructor.  If there are spaces
      // in the provided identifier, they are ignored
   RinexObsID RinexObsID ::
   newID(const std::string& strID, const std::string& desc)
   {
      if (char2ot.count(strID[0]) && 
          char2cb.count(strID[1]) && 
          char2tc.count(strID[2]))
      {
         GPSTK_THROW(InvalidParameter("Identifier " + strID +
                                      " already defined."));
      }

      return idCreator(strID, desc);
   }


   RinexObsID RinexObsID ::
   idCreator(const std::string& strID, const std::string& desc)
   {
      char ot = strID[0];
      ObservationType type;
      if (!char2ot.count(ot))
      {
         type = (ObservationType)otDesc.size();
         otDesc[type] = desc;
         char2ot[ot] = type;
         ot2char[type] = ot;
      }
      else
         type = char2ot[ot];

      char cb = strID[1];
      CarrierBand band;
      if (!char2cb.count(cb))
      {
         band = (CarrierBand)cbDesc.size();
         cbDesc[band] = desc;
         char2cb[cb] = band;
         cb2char[band] = cb;
      }
      else
         band = char2cb[cb];

      char tc = strID[2];
      TrackingCode code;
      if (!char2tc.count(tc))
      {
         code = (TrackingCode) tcDesc.size();
         tcDesc[code] = desc;
         char2tc[tc] = code;
         tc2char[code] = tc;
      }
      else
         code = char2tc[tc];
 
      return RinexObsID(type, band, code);
   }
}  // end namespace
