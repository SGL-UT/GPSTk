/** @warning This code is automatically generated.
 *
 *  DO NOT EDIT THIS CODE BY HAND.
 *
 *  Refer to the documenation in the toolkit_docs gitlab project.
 */

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
//  This software developed by Applied Research Laboratories at the University
//  of Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#include "RinexObsID.hpp"

namespace gpstk
{
      // Mappings between code, carrier, type and characters.
      // The following definitions really should only describe the
      // items that are in the Rinex 3 specification. If an
      // application needs additional ObsID types to be able to be
      // translated to/from Rinex3, the additional types must be added
      // by the application.
   std::map<char, ObservationType> RinexObsID::char2ot {
      { ' ',  ObservationType::Unknown },
      { '*',  ObservationType::Any },
      { 'C',  ObservationType::Range },
      { 'L',  ObservationType::Phase },
      { 'D',  ObservationType::Doppler },
      { 'S',  ObservationType::SNR },
      { 'X',  ObservationType::Channel },
      { 'I',  ObservationType::Iono },
      { '-',  ObservationType::Undefined },
   }; // std::map<char, ObservationType> RinexObsID::char2ot

   std::map<char, CarrierBand> RinexObsID::char2cb {
      { ' ',  CarrierBand::Unknown },
      { '*',  CarrierBand::Any },
      { '1',  CarrierBand::L1 },
      { '2',  CarrierBand::L2 },
      { '5',  CarrierBand::L5 },
      { '4',  CarrierBand::G1a },
      { '6',  CarrierBand::G2a },
      { '3',  CarrierBand::G3 },
      { '7',  CarrierBand::E5b },
      { '8',  CarrierBand::E5ab },
      { '9',  CarrierBand::I9 },
      { '-',  CarrierBand::Undefined },
   }; // std::map<char, CarrierBand> RinexObsID::char2cb

   std::map<char, TrackingCode> RinexObsID::char2tc {
      { ' ',  TrackingCode::Unknown },
      { '*',  TrackingCode::Any },
      { 'C',  TrackingCode::CA },
      { 'P',  TrackingCode::P },
      { 'Y',  TrackingCode::Y },
      { 'W',  TrackingCode::W },
      { 'N',  TrackingCode::N },
      { 'D',  TrackingCode::D },
      { 'M',  TrackingCode::M },
      { 'S',  TrackingCode::C2M },
      { 'L',  TrackingCode::C2L },
      { 'X',  TrackingCode::C2LM },
      { 'I',  TrackingCode::I5 },
      { 'Q',  TrackingCode::Q5 },
      { 'A',  TrackingCode::L1OCD },
      { 'B',  TrackingCode::L1OCP },
      { 'Z',  TrackingCode::ABC },
      { 'E',  TrackingCode::JE6 },
      { '-',  TrackingCode::Undefined },
   }; // std::map<char, TrackingCode> RinexObsID::char2tc

   std::map<ObservationType, char> RinexObsID::ot2char {
      { ObservationType::Unknown,    ' ' },
      { ObservationType::Any,        '*' },
      { ObservationType::Range,      'C' },
      { ObservationType::Phase,      'L' },
      { ObservationType::Doppler,    'D' },
      { ObservationType::SNR,        'S' },
      { ObservationType::Channel,    'X' },
      { ObservationType::Iono,       'I' },
      { ObservationType::Undefined,  '-' },
   }; // std::map<ObservationType, char> RinexObsID::ot2char

   std::map<CarrierBand, char> RinexObsID::cb2char {
      { CarrierBand::Unknown,    ' ' },
      { CarrierBand::Any,        '*' },
      { CarrierBand::L1,         '1' },
      { CarrierBand::L2,         '2' },
      { CarrierBand::L5,         '5' },
      { CarrierBand::G1,         '1' },
      { CarrierBand::G1a,        '4' },
      { CarrierBand::G2a,        '6' },
      { CarrierBand::G2,         '2' },
      { CarrierBand::G3,         '3' },
      { CarrierBand::E5b,        '7' },
      { CarrierBand::E5ab,       '8' },
      { CarrierBand::E6,         '6' },
      { CarrierBand::B1,         '2' },
      { CarrierBand::B2,         '7' },
      { CarrierBand::B3,         '6' },
      { CarrierBand::I9,         '9' },
      { CarrierBand::Undefined,  '-' },
   }; // std::map<CarrierBand, char> RinexObsID::cb2char

   std::map<TrackingCode, char> RinexObsID::tc2char {
      { TrackingCode::Unknown,    ' ' },
      { TrackingCode::Any,        '*' },
      { TrackingCode::CA,         'C' },
      { TrackingCode::P,          'P' },
      { TrackingCode::Y,          'Y' },
      { TrackingCode::W,          'W' },
      { TrackingCode::N,          'N' },
      { TrackingCode::D,          'D' },
      { TrackingCode::M,          'M' },
      { TrackingCode::C2M,        'S' },
      { TrackingCode::C2L,        'L' },
      { TrackingCode::C2LM,       'X' },
      { TrackingCode::I5,         'I' },
      { TrackingCode::Q5,         'Q' },
      { TrackingCode::IQ5,        'X' },
      { TrackingCode::G1P,        'L' },
      { TrackingCode::G1D,        'S' },
      { TrackingCode::G1X,        'X' },
      { TrackingCode::GCA,        'C' },
      { TrackingCode::GP,         'P' },
      { TrackingCode::IR3,        'I' },
      { TrackingCode::QR3,        'Q' },
      { TrackingCode::IQR3,       'X' },
      { TrackingCode::L1OCD,      'A' },
      { TrackingCode::L1OCP,      'B' },
      { TrackingCode::L1OC,       'X' },
      { TrackingCode::L2CSIOCp,   'X' },
      { TrackingCode::L2CSI,      'A' },
      { TrackingCode::L2OCP,      'B' },
      { TrackingCode::A,          'A' },
      { TrackingCode::B,          'B' },
      { TrackingCode::C,          'C' },
      { TrackingCode::BC,         'X' },
      { TrackingCode::ABC,        'Z' },
      { TrackingCode::IE5,        'I' },
      { TrackingCode::QE5,        'Q' },
      { TrackingCode::IQE5,       'X' },
      { TrackingCode::IE5a,       'I' },
      { TrackingCode::QE5a,       'Q' },
      { TrackingCode::IQE5a,      'X' },
      { TrackingCode::IE5b,       'I' },
      { TrackingCode::QE5b,       'Q' },
      { TrackingCode::IQE5b,      'X' },
      { TrackingCode::A6,         'A' },
      { TrackingCode::B6,         'B' },
      { TrackingCode::C6,         'C' },
      { TrackingCode::BC6,        'X' },
      { TrackingCode::ABC6,       'Z' },
      { TrackingCode::SCA,        'C' },
      { TrackingCode::SI5,        'I' },
      { TrackingCode::SQ5,        'Q' },
      { TrackingCode::SIQ5,       'X' },
      { TrackingCode::JCA,        'C' },
      { TrackingCode::JD1,        'S' },
      { TrackingCode::JP1,        'L' },
      { TrackingCode::JX1,        'X' },
      { TrackingCode::JZ1,        'Z' },
      { TrackingCode::JM2,        'S' },
      { TrackingCode::JL2,        'L' },
      { TrackingCode::JX2,        'X' },
      { TrackingCode::JI5,        'I' },
      { TrackingCode::JQ5,        'Q' },
      { TrackingCode::JIQ5,       'X' },
      { TrackingCode::JI5S,       'D' },
      { TrackingCode::JQ5S,       'P' },
      { TrackingCode::JIQ5S,      'Z' },
      { TrackingCode::JI6,        'S' },
      { TrackingCode::JQ6,        'L' },
      { TrackingCode::JIQ6,       'X' },
      { TrackingCode::JD6,        'S' },
      { TrackingCode::JE6,        'E' },
      { TrackingCode::JDE6,       'Z' },
      { TrackingCode::CI1,        'I' },
      { TrackingCode::CQ1,        'Q' },
      { TrackingCode::CIQ1,       'X' },
      { TrackingCode::CI7,        'I' },
      { TrackingCode::CQ7,        'Q' },
      { TrackingCode::CIQ7,       'X' },
      { TrackingCode::CI6,        'I' },
      { TrackingCode::CQ6,        'Q' },
      { TrackingCode::CIQ6,       'X' },
      { TrackingCode::CA1,        'A' },
      { TrackingCode::CCD1,       'D' },
      { TrackingCode::CCDP1,      'X' },
      { TrackingCode::CCP1,       'P' },
      { TrackingCode::CI2ab,      'D' },
      { TrackingCode::CIQ2ab,     'X' },
      { TrackingCode::CQ2ab,      'P' },
      { TrackingCode::CI2a,       'D' },
      { TrackingCode::CIQ2a,      'X' },
      { TrackingCode::CQ2a,       'P' },
      { TrackingCode::CI2b,       'D' },
      { TrackingCode::CIQ2b,      'Z' },
      { TrackingCode::CQ2b,       'P' },
      { TrackingCode::CodelessC,  'N' },
      { TrackingCode::CIQ3A,      'A' },
      { TrackingCode::IA5,        'A' },
      { TrackingCode::IB5,        'B' },
      { TrackingCode::IC5,        'C' },
      { TrackingCode::IX5,        'X' },
      { TrackingCode::IA9,        'A' },
      { TrackingCode::IB9,        'B' },
      { TrackingCode::IC9,        'C' },
      { TrackingCode::IX9,        'X' },
      { TrackingCode::Undefined,  '-' },
   }; // std::map<TrackingCode, char> RinexObsID::tc2char

   // map of valid RINEX tracking codes, systems and frequency
   std::map<char, std::map<char, std::string> >
   RinexObsID::validRinexTrackingCodes {
      { 'G', {
               // 1N: Pseudorange (C) not valid
            { '1', "PYWLMCSXN* " },
               // 2N: Pseudorange (C) not valid
            { '2', "PYWLMCSXDN* " },
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

      { 'J', {
            { '1', "CSLXZ* " },
            { '2', "SLX* " },
            { '5', "IQDPXZ* " },
            { '6', "ESLXZ* " } } },

      { 'C', {
               // 1I: RINEX 3.02 kludge
               // 1Q: RINEX 3.02 kludge
            { '1', "PAIQXDN* " },
            { '2', "IQX* " },
            { '5', "PXD* " },
            { '6', "AIQX* " },
            { '7', "ZPIQXD* " },
            { '8', "PXD* " } } },

      { 'I', {
            { '1', "* " }, // I1 is *only* for channel number
            { '5', "ABCX* " },
            { '9', "ABCX* " } } }
   }; // RinexObsID::validRinexTrackingCodes

   RinexObsID::RinexObsID(const std::string& strID, double version)
         : rinexVersion(version)
   {
      std::string modStrID(strID);
      type = ObservationType::Unknown;
      band = CarrierBand::Unknown;
      code = TrackingCode::Unknown;
      int i = modStrID.length() - 3;
      if (i < 0 || i > 1)
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
      } // switch (modStrID[0])
   } // RinexObsID(const std::string& strID, double version)
} // namespace gpstk

